/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/


#ifndef COMMANDSETS_HPP_INCLUDE_GUARD_8K38ID7884D343DWKH
#define COMMANDSETS_HPP_INCLUDE_GUARD_8K38ID7884D343DWKH

#include "DataSet.hpp"
namespace dicom
{

	//!Command sets, as defined in Part 7, Section 9
	/*!
		These are created by the client or the server, fed 'down the wire',
		reconstructed at the other end and then acted upon.  As far as I can
		make out, (see Part 7, Figure 6.3-1), these can be understood as data sets
		with Little Endian byte ordering and Implicit VR encoding.
	*/

	namespace CommandSet
	{

		//////////////////////////////////////////////////////////////////////////
		//C-DIMSE commands
		//////////////////////////////////////////////////////////////////////////

		class CEchoRQ : public DataSet
		{
		public:
			CEchoRQ(UINT16 msgID, const UID& classUID);
		};

		class CEchoRSP : public DataSet
		{
		public:
			CEchoRSP(UINT16 msgID, const UID& classUID);
		};



		//!Request a C-STORE operation
		/*!
			Defined in Part 7, table 9.1-1
		*/
		class CStoreRQ : public DataSet
		{
		public:
			CStoreRQ(UINT16 msgID, const UID& classUID, const UID& instUID,
				UINT16 priority = Priority::MEDIUM);
			CStoreRQ(UINT16 msgID, const UID& classUID, const UID& instUID,
				const std::string& moveAET, UINT16 moveMsgID,
				UINT16 priority = Priority::MEDIUM);
		};

		//!Respond to a C-STORE Request
		/*!
			Defined in Part 7, table 9.1-1
		*/
		class CStoreRSP : public DataSet
		{
		public:
			CStoreRSP(UINT16 msgID, const UID& classUID, const UID& instUID, UINT16 stat);
		};

		//!C-FIND request.
		/*!
			Defined in Part 7, table 9.1-2
		*/
		class CFindRQ : public DataSet
		{
		public:
			CFindRQ(UINT16 msgID, const UID& classUID, UINT16 priority = Priority::MEDIUM);
		};

		//!C-FIND Response
		/*!
			Defined in Part 7, table 9.1-2
		*/
		class CFindRSP : public DataSet
		{
		public:
			CFindRSP(UINT16 msgID, const UID& classUID, UINT16 stat, UINT16 dsType);
		};

		class CCancelRQ : public DataSet
		{
		public:
			CCancelRQ(UINT16 msgID);
		};

		class CGetRQ : public DataSet
		{
		public:
			CGetRQ(UINT16 msgID, const UID& classUID, UINT16 priority = Priority::MEDIUM);
		};

		class CGetRSP : public DataSet
		{
		public:
			CGetRSP(UINT16 msgID, const UID& classUID, UINT16 stat, UINT16 dsType);
			void setRemaining(UINT16 n);
			void setCompleted(UINT16 n);
			void setFailed(UINT16 n);
			void setWarning(UINT16 n);
		};

		class CMoveRQ : public DataSet
		{
		public:
			CMoveRQ(UINT16 msgID, const UID& classUID, const std::string& destAET,
				UINT16 priority = Priority::MEDIUM);
		};

		class CMoveRSP : public DataSet
		{
		public:
			CMoveRSP(UINT16 msgID, const UID& classUID, UINT16 stat, UINT16 dsType);
			void setRemaining(UINT16 n);
			void setCompleted(UINT16 n);
			void setFailed(UINT16 n);
			void setWarning(UINT16 n);
		};

		//////////////////////////////////////////////////////////////////////////
		//N-DIMSE commands
		//////////////////////////////////////////////////////////////////////////


		class NEventReportRQ : public dicom::DataSet
		{
		public:
			NEventReportRQ(UINT16 msgID, const dicom::UID& classUID, const dicom::UID& instUID,
				UINT16 eventTypID, UINT16 dsType);
		};

		class NEventReportRSP : public dicom::DataSet
		{
		public:
			NEventReportRSP(UINT16 msgID, const dicom::UID& classUID, UINT16 stat,
				UINT16 eventTypID, UINT16 dsType);
		};

		class NGetRQ : public dicom::DataSet
		{
		public:
			NGetRQ(UINT16 msgID, const dicom::UID& classUID, const dicom::UID& instUID,
				const std::vector<Tag>& attrList);
		};

		class NGetRSP : public dicom::DataSet
		{
		public:
			NGetRSP(UINT16 msgID, const dicom::UID& classUID, UINT16 stat, UINT16 dsType);
		};

		class NSetRQ : public dicom::DataSet
		{
		public:
			NSetRQ(UINT16 msgID, const dicom::UID& classUID, const dicom::UID& instUID);
		};

		class NSetRSP : public dicom::DataSet
		{
		public:
			NSetRSP(UINT16 msgID, const dicom::UID& classUID, UINT16 stat, UINT16 dsType);
		};

		class NActionRQ : public dicom::DataSet
		{
		public:
			NActionRQ(UINT16 msgID, const dicom::UID& classUID, const dicom::UID& instUID,
				UINT16 actionTypID, UINT16 dsType);
		};

		class NActionRSP : public dicom::DataSet
		{
		public:
			NActionRSP(UINT16 msgID, const dicom::UID& classUID, UINT16 stat,
				UINT16 actionTypID, UINT16 dsType);
		};

		class NCreateRQ : public dicom::DataSet
		{
		public:
			NCreateRQ(UINT16 msgID, const dicom::UID& classUID, UINT16 dsType);
			NCreateRQ(UINT16 msgID, const dicom::UID& classUID, const dicom::UID& instUID,
				UINT16 dsType);
		};

		class NCreateRSP : public dicom::DataSet
		{
		public:
			NCreateRSP(UINT16 msgID, const dicom::UID& classUID, const dicom::UID& instUID,
				UINT16 stat, UINT16 dsType);
		};

		class NDeleteRQ : public dicom::DataSet
		{
		public:
			NDeleteRQ(UINT16 msgID, const dicom::UID& classUID, const dicom::UID& instUID);
		};

		class NDeleteRSP : public dicom::DataSet  
		{
		public:
			NDeleteRSP(UINT16 msgID, const dicom::UID& classUID, UINT16 stat);
		};
	}//namespace CommandSet

/*---------------------------------------------------------------------------
End of command sets.
---------------------------------------------------------------------------*/
}//namespace dicom
#endif //COMMANDSETS_HPP_INCLUDE_GUARD_8K38ID7884D343DWKH
