/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#ifndef AARJ_INCLUDE_GUARD_KD27IU34D2I3IR
#define AARJ_INCLUDE_GUARD_KD27IU34D2I3IR

#include "socket/Socket.hpp"
#include "Types.hpp"
namespace dicom
{

	namespace primitive
	{
		/*!
			Defined in Part 8, table 9-21
		*/
		class	AAssociateRJ
		{
		private:
			const static BYTE		ItemType_=0x03;
			const static BYTE		Reserved1_ = 0x00;
			const static UINT32		Length_ = 0x04;
			const static BYTE		Reserved2_ = 0x00;
		public:
			BYTE		Result_;
			BYTE		Source_;
			BYTE		Reason_;

			//!Constructor
						AAssociateRJ(BYTE Result, BYTE Source, BYTE Reason);
						AAssociateRJ();

			void		Write(Network::Socket &);
		//	void		Read(Network::Socket &);
			void		ReadDynamic(Network::Socket	&);


			//These could be enums that got converted to BYTEs in the constructor.

			//for reference see Part 8/table 9-21

			//Result
			const static BYTE 	 REJECTED_PERMANENT					= 1
								,REJECTED_TRANSIENT					= 2;
			//source
			const static BYTE 	 DICOM_SERVICE_USER					= 1
								,DICOM_SERVICE_PROVIDER_ACSE		= 2
								,DICOM_SERVICE_PROVIDER_PRESENTATION= 3;
			//reason
			const static BYTE	 RESERVED							= 0
								//source has value 1
								,NO_REASON							= 1
								,APPLICATION_CONTEXT_NOT_SUPPORTED	= 2
								,CALLING_AE_NOT_RECOGNIZED			= 3
								,CALLED_AE_NOT_RECOGNIZED			= 7
								//source has value 2
								,PROTOCOL_VERSION_NOT_SUPPORTED		= 2
								//source has value 3
								,TEMPORARY_CONGESTION				= 1
								,LOCAL_LIMIT_EXCEEDED				= 2;

	};

	/*!
		Defined in Part 8, table 9-24
	*/
		class	AReleaseRQ
		{
		private:
			const static BYTE		ItemType_ 	= 0x05;
			const static BYTE		Reserved1_ 	= 0x00;
			const static UINT32		Length_		= 0x04;
			const static UINT32		Reserved2_ 	= 0x00;
		public:

			void		Write(Network::Socket &);
			//void		Read(Network::Socket &);
			void		ReadDynamic(Network::Socket	&);
		};

		
		/*!
			Defined in Part8, table 9-25
		*/
		class	AReleaseRP
		{
		private:
			const static BYTE		ItemType_	= 0x06;
			const static BYTE		Reserved1_	= 0x00;
			const static UINT32		Length_		= 0x04;
			const static UINT32		Reserved2_	= 0x00;
		public:
			void		Write(Network::Socket &);
			void		Read(Network::Socket &);
			void		ReadDynamic(Network::Socket	&);
		};

		/*!
			Defined in Part 8, table 9-26
			Should be called just 'AAbort'
		*/
		class	AAbortRQ
		{
		private:
			const static BYTE		ItemType_ = 0x07;
			const static BYTE		Reserved1_ = 0x00;
			const static UINT32		Length_ = 0x04;
			const static BYTE		Reserved2_ = 0x00;
			const static BYTE		Reserved3_ = 0x00;
		public:
			BYTE		Source_;
			BYTE		Reason_;

						AAbortRQ(Network::Socket& socket);
						AAbortRQ(BYTE Source, BYTE Reason);

			void		Write(Network::Socket &);
			//void		Read(Network::Socket &);
			void		ReadDynamic(Network::Socket	&);


			//source (who  initiated the abort.)
			const static BYTE	 DICOM_SERVICE_USER					= 0
								,DICOM_SERVICE_PROVIDER				= 2;
			//reason (should be 0 if Source is 0)
			const static BYTE	 NO_REASON							= 0
								,UNRECOGNIZED_PDU					= 1
								,UNEXPECTED_PDU						= 2
								,UNRECOGNIZED_PDU_PARAMETER			= 4
								,UNEXPECTED_PDU_PARAMETER			= 5
								,INVALID_PDU_PARAMETER				= 6;
		};
	}//namespace primitive
}//namespace dicom

#endif //AARJ_INCLUDE_GUARD_KD27IU34D2I3IR
