/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/
#include <iostream>
#include "socket/Socket.hpp"
#include "UIDs.hpp"
#include "ImplementationUID.hpp"
#include "ClientConnection.hpp"
#include "aarj.hpp"
#include "AssociationRejection.hpp"
#include "Cdimse.hpp"

using std::cout;using std::endl;

/*
As I understand it, the way UCDMC99 works is this:
The user specifies a list of AbstractSyntaxes.

The Library converts this into a list of proposed PresentationContexts,
each having only one Transfer Syntax, of type Implicit VR/Little Endian.
This list get's sent to the server....

I'm a bit confused why UCDMC99 has all this endian handling stuff
and then only accepts one Transfer Syntax.  Why not support the rest?


*/

namespace dicom
{
	using namespace primitive;

	ClientConnection::ClientConnection(std::string Host, unsigned short Port,
		std::string LocalAET,std::string RemoteAET,
		//const std::vector<PresentationContext>& ProposedPresentationContexts)
		const PresentationContexts& ProposedPresentationContexts)
		//: ServiceBase(new Network::ClientSocket(Host,Port))
        : socket_(new Network::ClientSocket(Host,Port))

	{

		AAssociateRQ& association_request=AAssociateRQ_;//BAD BAD BAD


		association_request.m_callingAppTitle = LocalAET;
		association_request.m_calledAppTitle = RemoteAET;

		//If we get this far, we have a valid TCP/IP connection,
		//so now negotiate the association...


		/*
			user must specify 1 or more PresentationContexts.
			framework must give these temporary IDS, and feed them
			onto request object.

			We propose a set of PresentationContexts, the other end selects
			which ones it likes and sends us back a list.
		*/

		association_request.m_proposedPresentationContexts = ProposedPresentationContexts;//expensive copy operation!


		//last bit to do is:
		UserInformation UserInfo;
		MaximumSubLength MaxSubLength;
		MaxSubLength.set(16384);	// we can do all DICOM can handle???
		UserInfo.m_impClass.m_UID = ImplementationClassUID;
		UserInfo.m_impVersion.m_name = ImplementationVersionName;
		UserInfo.setMax(MaxSubLength);

		association_request.setUserInformation(UserInfo);

		association_request.write(*socket_);

		//examine response from server.
		BYTE ItemType;
		(*socket_) >> ItemType;

		switch(ItemType)
		{
		case	0x02:
			{
				AAssociateAC acknowledgement;
				acknowledgement.readDynamic(*socket_);

				if(!InterogateAAssociateAC(acknowledgement))
				{
					throw FailedAssociation();//need a more detailed error here.
				}
				return;//negotiation succesful!
			}
		case	0x03:
			{

				AAssociateRJ rejection;
				rejection.readDynamic(*socket_);
				throw AssociationRejection(rejection.m_result,rejection.m_source,rejection.m_reason);
			}
		default:

			throw BadItemType(ItemType,0);
		}

		//once we've finished negotiating, ServiceBase::AcceptedPresentationContexts_ will
		//have correct values.

	}

	/*
		There are some problems with this, for example if
		the client connection is getting destroyed as part of
		a stack unwind caused by an exception thrown on a failed
		link.
	*/

	ClientConnection::~ClientConnection()
	{

		//Try to negotiate a clean release with the server...
		try
		{
			AReleaseRQ release_request;
			release_request.write(*socket_);
			AReleaseRP response;
			response.read(*socket_);//should we do anything with this?
		}
		catch(std::exception& e)
		{
			cout << "Exception thrown in ClientConnection destructor:"<<e.what() << endl;
		}
		catch(...)//can't allow destructors to emit exceptions
		{
			cout << "Unknown exception throw in ClientConnection destructor." << endl;
			throw;//should actually be a terminate(), I think.
		}

		//delete socket_;
	}

	bool IsBad(PresentationContextAccept& PCA)
	{
		return (PCA.m_result != 0);
	}

	/*!
		copy all succesfully proposed PresentationContexts onto
		AcceptedPresentationContexts_
	*/
	bool ClientConnection::InterogateAAssociateAC(AAssociateAC& acknowledgement)
	{
		//"Accepted" means feed back from remote server. Not necessary "result is 0" -Sam
		AcceptedPresentationContexts_=acknowledgement.m_presContextAccepts;
		//AcceptedPresentationContexts_.clear();


        
        AcceptedPresentationContexts_ = acknowledgement.m_presContextAccepts;

        int unaccepted = std::count_if(acknowledgement.m_presContextAccepts.begin(),acknowledgement.m_presContextAccepts.end(),IsBad);
        return (AcceptedPresentationContexts_.size()>unaccepted);

	}


	/*
		This implies we need to add similar functions for the other operatons, such as MOVE
	*/
	//caller has not right to define ts here. It is determined by AcceptedPresentationContexts -Sam
	DataSet ClientConnection::Store(const DataSet& data/*,TS ts*/)
	{
		UID classUID(data(TAG_SOP_CLASS_UID).Get<UID>());
		UID instUID(data(TAG_SOP_INST_UID).Get<UID>());

		//Check the SOPClass in data and find the accepted transfer syntax
		BYTE presid;
		try
		{
			presid=GetPresentationContextID(classUID);
		}
		catch (dicom::exception& e)
		{
			cout << "In ClientConnection::Store: " << e.what();
		}
		SetCurrentPCID(presid);
		//commented out because the ts is handled inside ServiceBase class. -Sam April 9, 2007
		//UID tsuid;
		//try
		//{
		//	tsuid=GetTransferSyntaxUID(presid);
		//}
		//catch (std::runtime_error& r)
		//{
		//	cout << "In ClientConnection::Store: " <<  r.what();
		//}

		/*
			maybe the following could be pushed into CStoreSCU
			-after all, how else would one ever use a CStoreSCU?
		*/

		UINT16 status;
		DataSet response;
		CStoreSCU storeSCU(*this,classUID);
		storeSCU.writeRQ(instUID, data/*,TS(tsuid)*/);
		storeSCU.readRSP(status,response);
		return response;
	}


	DataSet ClientConnection::Move(const std::string& destination,const DataSet& query,QueryRetrieve::Root root)
	{
		UID classUID;
		switch(root)
		{
		case QueryRetrieve::STUDY_ROOT:
			classUID=STUDY_ROOT_QR_MOVE_SOP_CLASS;
			break;
		case QueryRetrieve::PATIENT_ROOT:
			classUID=PATIENT_ROOT_QR_MOVE_SOP_CLASS;
			break;
		case QueryRetrieve::PATIENT_STUDY_ONLY:
			classUID=PATIENT_STUDY_ONLY_QR_MOVE_SOP_CLASS;
			break;
		default:
			throw dicom::exception("Unknown QR root specified.");
		}

		//build a C-MOVE identifier...
		CMoveSCU moveSCU(*this, classUID);
		moveSCU.writeRQ(destination, query);
		UINT16 status = Status::PENDING;
		DataSet response;
        
        while (status==Status::PENDING || status == Status::PENDING1)
		{
          moveSCU.readRSP(status, response);
        }
		return response;
	}


	/*
		This utility function waits until all responses have been
		sent back, then returns them in a vector.  If you want to
		process each response as it comes in, you'll have to
		implement your own function, which should be straightforward
		if you borrow the code here.
	*/
	std::vector<DataSet> ClientConnection::Find(const DataSet& Query,QueryRetrieve::Root root)
	{
		UID classUID;
		switch(root)
		{
		case QueryRetrieve::STUDY_ROOT:
			classUID=STUDY_ROOT_QR_FIND_SOP_CLASS;
			break;
		case QueryRetrieve::PATIENT_ROOT:
			classUID=PATIENT_ROOT_QR_FIND_SOP_CLASS;
			break;
		case QueryRetrieve::PATIENT_STUDY_ONLY:
			classUID=PATIENT_STUDY_ONLY_QR_FIND_SOP_CLASS;
			break;
		case QueryRetrieve::MODALITY_WORKLIST:
			classUID=MODALITY_WORKLIST_SOP_CLASS;
			break;
		case QueryRetrieve::GENERAL_PURPOSE_WORKLIST:
			classUID=GENERAL_PURPOSE_WORKLIST_SOP_CLASS;
			break;
		default:
			throw dicom::exception("Unknown QR root specified.");
		}
		//Check the SOPClass in data and find the accepted transfer syntax
		BYTE presid;
		try
		{
			presid=GetPresentationContextID(classUID);
		}
		catch (dicom::exception& e)
		{
			cout << "In ClientConnection::Store: " << e.what();
		}
		SetCurrentPCID(presid);

		CFindSCU findSCU(*this,classUID);

		findSCU.writeRQ(Query);

		UINT16 status = Status::PENDING;
		std::vector<DataSet> Responses;
		while (status==Status::PENDING || status == Status::PENDING1)
		{
			DataSet response;
			DataSet data;
			findSCU.readRSP(status,response,data);
			if(data.size())
				Responses.push_back(data);
		}
		return Responses;

	}

	DataSet ClientConnection::Echo()
	{
		CEchoSCU echoSCU(*this);
		echoSCU.writeRQ();
		DataSet response;
		UINT16 status;
		echoSCU.readRSP(status,response);//should we do something with status?
		return response;

	}
	/*
		C-GET design.

		C-GET is harder to implement, as we have to recieve the images on the
		same connection as we make the request.

		So the procedure has to be:
		Write request.
		Listen for and process returned datasets.
		Read response.


		It's actually even more complicated than this, because the server
		might be sending back 'pending' responses.

		Do we need any callback tricks for this?
	*/
#if 0
	DataSet ClientConnection::Get(const DataSet& query,QueryRetrieve::Root root)
	{
		UID classUID=STUDY_ROOT_QR_GET_SOP_CLASS;// or whatever.

		//build a C-MOVE identifier...
		CGetSCU getSCU(*this,classUID);
		getSCU.writeRQ(query);

		/*
			Now at this point we need to start listening for C-STORE operations
			on THIS connection!  Can we just call HandleCStore, or something?

		*/
		Command::Code cmd;
		UID classUID;

		while(true)
		{
			DataSet command;
			Read(command);

			//now command may be a C-STORE-RQ, or
			//it may be a response to the C-GET-RQ!
			command(TAG_CMD_FIELD) >> cmd;
			command(TAG_AFF_SOP_CLASS_UID) >> classUID;

			if(/*it's a response*/)
			{
				if(/*it's pending*/)
					;//fine, carry on
				else if(/*it's completed*/
					break;
				else//something's wrong
					throw;
			}
			else
			{
				C_STORE_RQ==cmd
				//make sure it's a C-STORE
				//if it is, handle it.
				HandlerFunction handler=server_.GetHandler(classUID);
				HandleCStore(handler,*this,command,classUID);
			}
		}


		UINT16 status;
		DataSet response;
		get.readRSP(status,response);
		return response;
	}
#endif //0
};//namespace dicom
