/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "aaac.hpp"
#include <algorithm>
#include <functional>


using namespace std;
/************************************************************************
*
* Presentation Context Accept
*
************************************************************************/

namespace dicom
{
	namespace primitive
	{
#ifndef _MSC_VER
		/*
		difference of opinion here between gcc and msvc as to whether constant static members
		need to be instantiated seperately from their declaration..
		(Are we sure about this?)
		*/
		const BYTE PresentationContextAccept::ItemType_;
		const BYTE PresentationContextAccept::Reserved1_;
		const BYTE PresentationContextAccept::Reserved2_;
		// 	const BYTE PresentationContextAccept::Reserved3_;
		const BYTE PresentationContextAccept::Reserved4_;

		const BYTE AAssociateAC::ItemType_;
		const BYTE AAssociateAC::Reserved1_;
		const UINT16 AAssociateAC::ProtocolVersion_;
		const UINT16 AAssociateAC::Reserved2_;
#endif


		namespace
		{
			/*
			These globals are only safe if we NEVER EVER actually trust the data on them.
			(i.e. only use them for reading reserved data from byte stream that we don't
			check the value on.)

			Using them for anything else may cause thread-related data corruption. 
			*/

			BYTE tmpBYTE;

			UINT16 tmpUINT16;
			UINT32 tmpUINT32;
		}

		PresentationContextAccept::PresentationContextAccept()
			:TrnSyntax_(UID(""))

		{}

		void PresentationContextAccept::Write(Network::Socket& socket)
		{
			socket << ItemType_;
			socket << Reserved1_;
			socket << UINT16(Size()-4);
			socket << PresentationContextID_;
			socket << Reserved2_;
			socket << Result_;
			socket << Reserved4_;
			TrnSyntax_.Write(socket);
		}

		UINT32 PresentationContextAccept::Read(Network::Socket& socket)
		{
			UINT32 byteread=0;
			BYTE b;
			socket >> b;
			byteread+=sizeof(b);
			EnforceItemType(b,ItemType_);

			byteread+=ReadDynamic(socket);
			return byteread;
		}

		/*!
		See Section 8, table 9-18 for desciption of the following fields.

		*/
		UINT32 PresentationContextAccept::ReadDynamic(Network::Socket &socket)
		{
			UINT32 byteread=0;
			socket >> tmpBYTE;				//Reserved
			byteread+=sizeof(tmpBYTE);
			UINT16 length;
			socket >> length;				//number of bytes from here to the end of the TransferSyntax item
			byteread+=sizeof(length);
			//As far as I can see this is redundant apart from as an error check.
			socket >> PresentationContextID_;
			socket >> tmpBYTE;				//Reserved
			socket >> Result_;				//between 0 and 4
			socket >> tmpBYTE;				//Reserved
			byteread+=sizeof(PresentationContextID_)+sizeof(tmpBYTE)+sizeof(Result_)+sizeof(tmpBYTE);
			
			byteread+=TrnSyntax_.Read(socket);

			Enforce(Size()-4==length);
			return byteread;
		}

		UINT16 PresentationContextAccept::Size()
		{
			return TrnSyntax_.Size() + 8;
		}

		/************************************************************************
		*
		* AAssociateAC Packet
		*
		************************************************************************/

		AAssociateAC::AAssociateAC()
			:AppContext_(UID(""))
		{
			std::fill(Reserved3_,Reserved3_+32,0);
		}


		AAssociateAC::AAssociateAC(const std::string& CallingApp, const std::string& CalledApp)
			:CalledAppTitle_(CalledApp)
			,CallingAppTitle_(CallingApp)
			,AppContext_(UID(""))//not at all happy about this...
		{
			std::fill(Reserved3_,Reserved3_+32,0);
		}

		//!I feel this should be happening in the constructor...

		void AAssociateAC::SetUserInformation(UserInformation	&User)
		{
			UserInfo_ = User;
		}

		void AAssociateAC::Write(Network::Socket	&socket)
		{
			
			socket << ItemType_;
			socket << Reserved1_;

			socket << UINT32(Size()-6);

			socket << ProtocolVersion_;
			socket << Reserved2_;

			char StringBuffer[16];

			std::fill(StringBuffer,StringBuffer+16,' ');
			std::copy(CalledAppTitle_.begin(),CalledAppTitle_.end(),StringBuffer);
			socket.Sendn(StringBuffer,16);

			std::fill(StringBuffer,StringBuffer+16,' ');
			std::copy(CallingAppTitle_.begin(),CallingAppTitle_.end(),StringBuffer);
			socket.Sendn(StringBuffer,16);
			socket.Sendn(Reserved3_,32);
			AppContext_.Write(socket);


			for_each(PresContextAccepts_.begin(),PresContextAccepts_.end(),
				WriteToSocket(socket));

			UserInfo_.Write(socket);
		}

		UINT32 AAssociateAC::Read(Network::Socket	&socket)
		{
			UINT32 byteread=0;
			BYTE b;
			socket >> b;
			byteread+=sizeof(b);
			EnforceItemType(b,ItemType_);
			byteread+=ReadDynamic(socket);
			return byteread;
		}

		UINT32 AAssociateAC::ReadDynamic(Network::Socket	&socket)
		{
			UINT32 byteread=0;
			UINT32 tmp_read=0;
			BYTE	TempByte;
			PresentationContextAccept	PresContextAccept;//should be inside loop.

			socket >> tmpBYTE;//Reserved1_;
			byteread+=sizeof(tmpBYTE);
		
			UINT32 length;
			socket >> length;

			socket >> tmpUINT16;
			/*
			verify that (tmpUINT16 bitand 0x01) is true; - see part 8, table 9-17
			*/

			socket >>	tmpUINT16;
			byteread+=sizeof(length)+2*sizeof(tmpUINT16);

			CalledAppTitle_.assign(16,' ');
			socket.Read(CalledAppTitle_);
			byteread+=16;

			StripTrailingWhitespace(CalledAppTitle_);

			CallingAppTitle_.assign(16,' ');

			socket.Read(CallingAppTitle_);
			byteread+=16;
			StripTrailingWhitespace(CallingAppTitle_);

			socket.Readn(Reserved3_,32);
			byteread+=32;
			int BytesLeftToRead = length - sizeof(UINT16) - sizeof(UINT16) - 16 - 16 - 32;
			while(BytesLeftToRead > 0)
			{
				socket >> TempByte;
				BytesLeftToRead-=sizeof(TempByte);
				byteread+=sizeof(TempByte);
				switch(TempByte)
				{
				case	0x50:	// user information
					tmp_read=UserInfo_.ReadDynamic(socket);
					byteread+=tmp_read;
					BytesLeftToRead = BytesLeftToRead - tmp_read;//UserInfo_.Size();
					break;
				case	0x21:
					tmp_read=PresContextAccept.ReadDynamic(socket);
					BytesLeftToRead = BytesLeftToRead - tmp_read;//PresContextAccept.Size();
					byteread+=tmp_read;
					PresContextAccepts_.push_back(PresContextAccept);
					break;
				case	0x10:
					tmp_read=AppContext_.ReadDynamic(socket);
					byteread+=tmp_read;
					BytesLeftToRead = BytesLeftToRead - tmp_read;//AppContext_.Size();
					break;
				default:
					throw BadItemType(TempByte,0);
				}
			}

			//This line is not enforcible because it may not be true-Sam 28Nov2007
			//Enforce(length==Size()-6);

			//This if will never happen because of the while loop above. -Sam 28Nov2007
			//if(BytesLeftToRead)
			//	throw dicom::exception("non-zero remaining bytes in AAssociateAC::ReadDynamic()");
			return byteread;
		}

		UINT32	AAssociateAC::Size()
		{
			UINT32 length;
			length = sizeof(UINT16) + sizeof(UINT16) + 16 + 16 + 32;
			length += AppContext_.Size();
			unsigned int Index = 0;

			Index = 0;
			while(Index < PresContextAccepts_.size())
			{
				length += PresContextAccepts_[Index].Size();
				++Index;
			}

			length += UserInfo_.Size();
			return (length + sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT32) );
		}
	}//	namespace primitive
}//namespace dicom
