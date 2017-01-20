/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "aarj.hpp"
#include "aarq.hpp"//for BadItemType
/*
NOTE: The 'ReadDynamic' member functions assume that the 'ItemType_' field
has already been read from the data stream, (hence the PDU type is known.)

*/
/************************************************************************
*
* AAssociateRJ Packet
*
************************************************************************/
namespace dicom
{
	namespace primitive
	{
#ifndef _MSC_VER
		/*
		difference of opinion here between gcc and msvc as to whether constant static members
		need to be instantiated seperately.
		*/
		const BYTE AAssociateRJ::ItemType_;
		const BYTE AAssociateRJ::Reserved1_;
		const UINT32 AAssociateRJ::Length_;
		const BYTE AAssociateRJ::Reserved2_;

		const BYTE AReleaseRQ::ItemType_;
		const BYTE AReleaseRQ::Reserved1_;
		const UINT32 AReleaseRQ::Length_;
		const UINT32 AReleaseRQ::Reserved2_;

		const BYTE AReleaseRP::ItemType_;
		const BYTE AReleaseRP::Reserved1_;
		const UINT32 AReleaseRP::Length_;
		const UINT32 AReleaseRP::Reserved2_;

		const BYTE AAbortRQ::ItemType_;
		const BYTE AAbortRQ::Reserved1_;
		const UINT32 AAbortRQ::Length_;
		const BYTE AAbortRQ::Reserved2_;
		const BYTE AAbortRQ::Reserved3_;
#endif






		namespace
		{
			BYTE tmpBYTE;
			UINT32 tmpUINT32;
		}
		AAssociateRJ::AAssociateRJ(BYTE Result, BYTE Source, BYTE Reason)
			:
		Result_(Result),
			Source_(Source),
			Reason_(Reason)
		{
		}


		AAssociateRJ::AAssociateRJ()
		{}


		void AAssociateRJ::Write(Network::Socket& socket)
		{
			socket << ItemType_;
			socket << Reserved1_;
			socket << Length_;
			socket << Reserved2_;
			socket << Result_;
			socket << Source_;
			socket << Reason_;
		}

		// 	void	AAssociateRJ::Read(Network::Socket	&socket)
		// 	{
		// 		socket.Readn((BYTE *) &ItemType_, sizeof(BYTE));
		// 		ReadDynamic(socket);
		// 	}

		void AAssociateRJ::ReadDynamic(Network::Socket& socket)
		{
			socket >> tmpBYTE;
			socket >> tmpUINT32;
			socket >> tmpBYTE;
			socket >> Result_;
			socket >> Source_;
			socket >> Reason_;
		}



		/************************************************************************
		*
		* AReleaseRQ Packet
		*
		************************************************************************/




		void AReleaseRQ::Write(Network::Socket& socket)
		{
			socket << ItemType_;
			socket << Reserved1_;
			socket << Length_;
			socket << Reserved2_;

		}

		// 	void	AReleaseRQ	::	Read(Network::Socket	&socket)
		// 	{
		// 		socket >> ItemType_;
		// 		ReadDynamic(socket);
		// 	}

		/*!
		Section 8, table 9.3.7
		*/
		void AReleaseRQ::ReadDynamic(Network::Socket& socket)
		{
			socket >> tmpBYTE;
			socket >> tmpUINT32;//should be 4 but we don't bother checking.
			socket >> tmpUINT32;

		}

		/************************************************************************
		*
		* AReleaseRP Packet
		*
		************************************************************************/



		void AReleaseRP::Write(Network::Socket& socket)
		{

			socket << ItemType_;
			socket << Reserved1_;
			socket << Length_;
			socket << Reserved2_;

		}

		void AReleaseRP::Read(Network::Socket& socket)
		{
			BYTE b;
			socket >> b;
			EnforceItemType(b,ItemType_);
			this->ReadDynamic(socket);
		}

		void AReleaseRP::ReadDynamic(Network::Socket& socket)
		{
			socket >> tmpBYTE;
			socket >> tmpUINT32;
			socket >> tmpUINT32;
		}



		/************************************************************************
		*
		* AAbortRQ Packet
		*
		************************************************************************/

		/*
			in most other cases, we first create the object and then
			call ReadDynamic(Socket&) on it.  It would be better
			to combine the two operations into one constructor as
			follows.  
		*/

		AAbortRQ::AAbortRQ(Network::Socket& socket)
		{
			//must have already read ItemType_ from stream!
			ReadDynamic(socket);
		}

		AAbortRQ::AAbortRQ(BYTE Source, BYTE Reason)
			:Source_(Source),Reason_(Reason)
		{
		}

		void AAbortRQ::Write(Network::Socket& socket)
		{
			socket << ItemType_;
			socket << Reserved1_;
			socket << Length_;
			socket << Reserved2_;
			socket << Reserved3_;
			socket << Source_;
			socket << Reason_;
		}

		void AAbortRQ::ReadDynamic(Network::Socket	&socket)
		{
			socket >> tmpBYTE;
			socket >> tmpUINT32;
			socket >> tmpBYTE;
			socket >> tmpBYTE;
			socket >> Source_;
			socket >> Reason_;
		}
	}//namespace primitive
}//namespace dicom
