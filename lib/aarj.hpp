/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#pragma once

#include "socket/Socket.hpp"
#include "Types.hpp"

namespace dicom
{
  namespace primitive
  {
    /*!
    Defined in Part 8, table 9-21
    */
    class AAssociateRJ
    {
      static const BYTE m_itemType = 0x03;
      static const BYTE m_reserved1 = 0x00;
      static const UINT32 m_length = 0x04;
      static const BYTE m_reserved2 = 0x00;

    public:

      BYTE m_result;
      BYTE m_source;
      BYTE m_reason;

      AAssociateRJ();
      AAssociateRJ(BYTE Result, BYTE Source, BYTE Reason);

      void write(Network::Socket&);
      void readDynamic(Network::Socket&);

      //These could be enums that got converted to BYTEs in the constructor.

      //for reference see Part 8/table 9-21

      //Result
      const static BYTE REJECTED_PERMANENT = 1;
      const static BYTE REJECTED_TRANSIENT = 2;
      //source
      const static BYTE DICOM_SERVICE_USER = 1;
      const static BYTE DICOM_SERVICE_PROVIDER_ACSE = 2;
      const static BYTE DICOM_SERVICE_PROVIDER_PRESENTATION = 3;
      //reason
      const static BYTE RESERVED = 0;
      //source has value 1
      const static BYTE NO_REASON = 1;
      const static BYTE APPLICATION_CONTEXT_NOT_SUPPORTED = 2;
      const static BYTE CALLING_AE_NOT_RECOGNIZED = 3;
      const static BYTE CALLED_AE_NOT_RECOGNIZED = 7;
      //source has value 2
      const static BYTE PROTOCOL_VERSION_NOT_SUPPORTED = 2;
      //source has value 3
      const static BYTE TEMPORARY_CONGESTION = 1;
      const static BYTE LOCAL_LIMIT_EXCEEDED = 2;
    };

    /*!
    Defined in Part 8, table 9-24
    */
    class AReleaseRQ
    {
      const static BYTE m_itemType = 0x05;
      const static BYTE m_reserved1 = 0x00;
      const static UINT32 m_length = 0x04;
      const static UINT32 m_reserved2 = 0x00;

    public:

      void write(Network::Socket&);
      void readDynamic(Network::Socket&);
    };

    /*!
    Defined in Part8, table 9-25
    */
    class AReleaseRP
    {
      const static BYTE m_itemType = 0x06;
      const static BYTE m_reserved1 = 0x00;
      const static UINT32 m_length = 0x04;
      const static UINT32 m_reserved2 = 0x00;

    public:

      void write(Network::Socket &);
      void read(Network::Socket &);
      void readDynamic(Network::Socket &);
    };

    /*!
    Defined in Part 8, table 9-26
    Should be called just 'AAbort'
    */
    class AAbortRQ
    {
      const static BYTE m_itemType = 0x07;
      const static BYTE m_reserved1 = 0x00;
      const static UINT32 m_length = 0x04;
      const static BYTE m_reserved2 = 0x00;
      const static BYTE m_reserved3 = 0x00;

    public:

      BYTE m_source;
      BYTE m_reason;

      AAbortRQ(Network::Socket& socket);
      AAbortRQ(BYTE Source, BYTE Reason);

      void write(Network::Socket&);
      void readDynamic(Network::Socket&);

      //source (who  initiated the abort.)
      const static BYTE DICOM_SERVICE_USER = 0;
      const static BYTE DICOM_SERVICE_PROVIDER = 2;
      //reason (should be 0 if Source is 0)
      const static BYTE NO_REASON = 0;
      const static BYTE UNRECOGNIZED_PDU = 1;
      const static BYTE UNEXPECTED_PDU = 2;
      const static BYTE UNRECOGNIZED_PDU_PARAMETER = 4;
      const static BYTE UNEXPECTED_PDU_PARAMETER = 5;
      const static BYTE INVALID_PDU_PARAMETER = 6;
    };
  }//namespace primitive
}//namespace dicom
