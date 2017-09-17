/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/
#pragma once

#include "Types.hpp"
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

      CEchoRQ(std::uint16_t msgID, const UID& classUID);
    };

    class CEchoRSP : public DataSet
    {

    public:

      CEchoRSP(std::uint16_t msgID, const UID& classUID);
    };

    //!Request a C-STORE operation
    /*!
    Defined in Part 7, table 9.1-1
    */
    class CStoreRQ : public DataSet
    {

    public:

      CStoreRQ(std::uint16_t msgID, const UID& classUID, const UID& instUID, std::uint16_t priority = Priority::MEDIUM);
      CStoreRQ(std::uint16_t msgID, const UID& classUID, const UID& instUID, const std::string& moveAET, std::uint16_t moveMsgID,
        std::uint16_t priority = Priority::MEDIUM);
    };

    //!Respond to a C-STORE Request
    /*!
    Defined in Part 7, table 9.1-1
    */
    class CStoreRSP : public DataSet
    {

    public:

      CStoreRSP(std::uint16_t msgID, const UID& classUID, const UID& instUID, std::uint16_t stat);
    };

    //!C-FIND request.
    /*!
    Defined in Part 7, table 9.1-2
    */
    class CFindRQ : public DataSet
    {

    public:

      CFindRQ(std::uint16_t msgID, const UID& classUID, std::uint16_t priority = Priority::MEDIUM);
    };

    //!C-FIND Response
    /*!
    Defined in Part 7, table 9.1-2
    */

    class CFindRSP : public DataSet
    {

    public:

      CFindRSP(std::uint16_t msgID, const UID& classUID, std::uint16_t stat, std::uint16_t dsType);
    };

    class CCancelRQ : public DataSet
    {

    public:

      CCancelRQ(std::uint16_t msgID);
    };

    class CGetRQ : public DataSet
    {

    public:

      CGetRQ(std::uint16_t msgID, const UID& classUID, std::uint16_t priority = Priority::MEDIUM);
    };

    class CGetRSP : public DataSet
    {

    public:

      CGetRSP(std::uint16_t msgID, const UID& classUID, std::uint16_t stat, std::uint16_t dsType);
      void setRemaining(std::uint16_t n);
      void setCompleted(std::uint16_t n);
      void setFailed(std::uint16_t n);
      void setWarning(std::uint16_t n);
    };

    class CMoveRQ : public DataSet
    {

    public:

      CMoveRQ(std::uint16_t msgID, const UID& classUID, const std::string& destAET, std::uint16_t priority = Priority::MEDIUM);
    };

    class CMoveRSP : public DataSet
    {

    public:

      CMoveRSP(std::uint16_t msgID, const UID& classUID, std::uint16_t stat, std::uint16_t dsType);
      void setRemaining(std::uint16_t n);
      void setCompleted(std::uint16_t n);
      void setFailed(std::uint16_t n);
      void setWarning(std::uint16_t n);
    };

    //////////////////////////////////////////////////////////////////////////
    //N-DIMSE commands
    //////////////////////////////////////////////////////////////////////////

    class NEventReportRQ : public dicom::DataSet
    {

    public:

      NEventReportRQ(std::uint16_t msgID, const dicom::UID& classUID, const dicom::UID& instUID,
        std::uint16_t eventTypID, std::uint16_t dsType);
    };

    class NEventReportRSP : public dicom::DataSet
    {

    public:

      NEventReportRSP(std::uint16_t msgID, const dicom::UID& classUID, std::uint16_t stat,
        std::uint16_t eventTypID, std::uint16_t dsType);
    };

    class NGetRQ : public dicom::DataSet
    {

    public:

      NGetRQ(std::uint16_t msgID, const dicom::UID& classUID, const dicom::UID& instUID, const std::vector<Tag>& attrList);
    };

    class NGetRSP : public dicom::DataSet
    {

    public:

      NGetRSP(std::uint16_t msgID, const dicom::UID& classUID, std::uint16_t stat, std::uint16_t dsType);
    };

    class NSetRQ : public dicom::DataSet
    {

    public:

      NSetRQ(std::uint16_t msgID, const dicom::UID& classUID, const dicom::UID& instUID);
    };

    class NSetRSP : public dicom::DataSet
    {

    public:

      NSetRSP(std::uint16_t msgID, const dicom::UID& classUID, std::uint16_t stat, std::uint16_t dsType);
    };

    class NActionRQ : public dicom::DataSet
    {

    public:

      NActionRQ(std::uint16_t msgID, const dicom::UID& classUID, const dicom::UID& instUID,
        std::uint16_t actionTypID, std::uint16_t dsType);
    };

    class NActionRSP : public dicom::DataSet
    {

    public:

      NActionRSP(std::uint16_t msgID, const dicom::UID& classUID, std::uint16_t stat,
        std::uint16_t actionTypID, std::uint16_t dsType);
    };

    class NCreateRQ : public dicom::DataSet
    {

    public:

      NCreateRQ(std::uint16_t msgID, const dicom::UID& classUID, std::uint16_t dsType);
      NCreateRQ(std::uint16_t msgID, const dicom::UID& classUID, const dicom::UID& instUID, std::uint16_t dsType);
    };

    class NCreateRSP : public dicom::DataSet
    {

    public:

      NCreateRSP(std::uint16_t msgID, const dicom::UID& classUID, const dicom::UID& instUID,
        std::uint16_t stat, std::uint16_t dsType);
    };

    class NDeleteRQ : public dicom::DataSet
    {

    public:

      NDeleteRQ(std::uint16_t msgID, const dicom::UID& classUID, const dicom::UID& instUID);
    };

    class NDeleteRSP : public dicom::DataSet  
    {

    public:

      NDeleteRSP(std::uint16_t msgID, const dicom::UID& classUID, std::uint16_t stat);
    };
  }//namespace CommandSet
}//namespace dicom
