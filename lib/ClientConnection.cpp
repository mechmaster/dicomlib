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
#include "aarj.hpp"
#include "AssociationRejection.hpp"
#include "Cdimse.hpp"

#include "ClientConnection.hpp"

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
  Network::Socket* ClientConnection::GetSocket()
  {
    return m_socket.get();
  }

  ClientConnection::ClientConnection(const std::string& Host, unsigned short Port, const std::string& LocalAET,
    const std::string& RemoteAET, const PresentationContexts& ProposedPresentationContexts) :
    m_socket(new Network::ClientSocket(Host, Port))
  {
    primitive::AAssociateRQ& association_request = AAssociateRQ_;//BAD BAD BAD

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
    primitive::UserInformation UserInfo;
    primitive::MaximumSubLength MaxSubLength;
    MaxSubLength.set(16384); // we can do all DICOM can handle???
    UserInfo.m_impClass.m_UID = ImplementationClassUID;
    UserInfo.m_impVersion.m_name = ImplementationVersionName;
    UserInfo.setMax(MaxSubLength);

    association_request.setUserInformation(UserInfo);
    association_request.write(*m_socket);

    //examine response from server.
    BYTE ItemType;
    (*m_socket) >> ItemType;

    switch(ItemType)
    {
      case 0x02:
        {
        primitive::AAssociateAC acknowledgement;
        acknowledgement.readDynamic(*m_socket);

        if(!interogateAAssociateAC(acknowledgement))
        {
          throw FailedAssociation();//need a more detailed error here.
        }

        return;//negotiation succesful!
        }
      case 0x03:
      {
        primitive::AAssociateRJ rejection;
        rejection.readDynamic(*m_socket);
        throw AssociationRejection(rejection.m_result, rejection.m_source, rejection.m_reason);
      }
      default:
        throw BadItemType(ItemType, 0);
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
      primitive::AReleaseRQ release_request;
      release_request.write(*m_socket);

      primitive::AReleaseRP response;
      response.read(*m_socket);//should we do anything with this?
    }
    catch(std::exception& e)
    {
      std::cout << "Exception thrown in ClientConnection destructor:" << e.what() << std::endl;
    }
    catch(...)//can't allow destructors to emit exceptions
    {
      std::cout << "Unknown exception throw in ClientConnection destructor." << std::endl;
    }
  }

  bool IsBad(primitive::PresentationContextAccept& PCA)
  {
    return (PCA.m_result != 0);
  }

  /*!
  copy all succesfully proposed PresentationContexts onto
  AcceptedPresentationContexts_
  */
  bool ClientConnection::interogateAAssociateAC(primitive::AAssociateAC& acknowledgement)
  {
    //"Accepted" means feed back from remote server. Not necessary "result is 0" -Sam
    AcceptedPresentationContexts_ = acknowledgement.m_presContextAccepts;

    int unaccepted = std::count_if(acknowledgement.m_presContextAccepts.begin(), acknowledgement.m_presContextAccepts.end(), IsBad);
    return (AcceptedPresentationContexts_.size() > unaccepted);
  }

  /*
  This implies we need to add similar functions for the other operatons, such as MOVE
  */
  //caller has not right to define ts here. It is determined by AcceptedPresentationContexts -Sam
  DataSet ClientConnection::store(const DataSet& data)
  {
    UID classUID(data(TAG_SOP_CLASS_UID).Get<UID>());
    UID instUID(data(TAG_SOP_INST_UID).Get<UID>());

    //Check the SOPClass in data and find the accepted transfer syntax
    BYTE presid;

    try
    {
      presid = GetPresentationContextID(classUID);
    }
    catch (dicom::exception& e)
    {
      std::cout << "In ClientConnection::Store: " << e.what() << std::endl;
    }

    SetCurrentPCID(presid);

    /*
    maybe the following could be pushed into CStoreSCU
    -after all, how else would one ever use a CStoreSCU?
    */

    UINT16 status;
    DataSet response;
    CStoreSCU storeSCU(*this, classUID);
    storeSCU.writeRQ(instUID, data);
    storeSCU.readRSP(status, response);
    return response;
  }

  DataSet ClientConnection::move(const std::string& destination, const DataSet& query, QueryRetrieve::Root root)
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

    while (status == Status::PENDING || status == Status::PENDING1)
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
  std::vector<DataSet> ClientConnection::find(const DataSet& Query, QueryRetrieve::Root root)
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
      presid = GetPresentationContextID(classUID);
    }
    catch (dicom::exception& e)
    {
      std::cout << "In ClientConnection::Store: " << e.what() << std::endl;
    }

    SetCurrentPCID(presid);

    CFindSCU findSCU(*this, classUID);
    findSCU.writeRQ(Query);

    UINT16 status = Status::PENDING;
    std::vector<DataSet> Responses;

    while (status==Status::PENDING || status == Status::PENDING1)
    {
      DataSet response;
      DataSet data;

      findSCU.readRSP(status, response, data);

      if(data.size())
      {
        Responses.push_back(data);
      }
    }

    return Responses;
  }

  DataSet ClientConnection::echo()
  {
    CEchoSCU echoSCU(*this);
    echoSCU.writeRQ();

    DataSet response;
    UINT16 status;

    echoSCU.readRSP(status, response);//should we do something with status?

    return response;
  }
};//namespace dicom
