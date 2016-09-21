/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/


#ifndef AAAC_HPP_INCLUDE_GUARD_K7763KDIOXBK2
#define AAAC_HPP_INCLUDE_GUARD_K7763KDIOXBK2

#include <vector>
#include "aarq.hpp"


namespace dicom
{
	//!message primitives, as described in Part 8, section 9
	namespace primitive
	{

		/*!
		Defined in Part 8, table 9-18
		*/
		struct	PresentationContextAccept
		{

			static const BYTE				ItemType_=0x21;
			static const BYTE				Reserved1_=0x00;
			//UINT16							Length_;
			BYTE							PresentationContextID_;
			static const BYTE				Reserved2_=0x00;
			BYTE							Result_;
			static const BYTE				Reserved4_=0x00;
		
			TransferSyntax				TrnSyntax_;
			PresentationContextAccept();
			//PresentationContextAccept(TransferSyntax &);

			void		Write(Network::Socket &);
			UINT32		Read(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT16		Size();
		};
		

		//!Should document where this is defined
		class	AAssociateAC
		{
		private:
			static const BYTE		ItemType_= 0x02;
			static const BYTE		Reserved1_=0x00;;
			static const UINT16	ProtocolVersion_	= 0x01;
			static const UINT16	Reserved2_		=0x00;
		public:

			std::string CalledAppTitle_;
			std::string CallingAppTitle_;
			BYTE		Reserved3_[32];
			ApplicationContext					AppContext_;
			std::vector<PresentationContextAccept>	PresContextAccepts_;
			UserInformation						UserInfo_;
		public:

			AAssociateAC();
			AAssociateAC(const std::string& CallingApp,const std::string& CalledApp);

			void		SetUserInformation(UserInformation &);
			void		Write(Network::Socket &);
			UINT32		Read(Network::Socket &);
			UINT32		ReadDynamic(Network::Socket	&);
			UINT32		Size();
		};			
	}//namespace primitive
}//namespace dicom

#endif //AAAC_HPP_INCLUDE_GUARD_K7763KDIOXBK2
