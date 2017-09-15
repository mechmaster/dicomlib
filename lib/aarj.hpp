/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#pragma once

#include "Types.hpp"
#include "Buffer.hpp"

namespace dicom
{
  namespace primitive
  {
    /*!
    Defined in Part 8, table 9-21
    */
    class AAssociateRJ
    {
      static const std::uint8_t m_itemType = 0x03;
      static const std::uint8_t m_reserved1 = 0x00;
      static const std::uint32_t m_length = 0x04;
      static const std::uint8_t m_reserved2 = 0x00;

    public:

      std::uint8_t m_result;
      std::uint8_t m_source;
      std::uint8_t m_reason;

      AAssociateRJ();
      AAssociateRJ(std::uint8_t Result, std::uint8_t Source, std::uint8_t Reason);

      void write(Buffer& temp);
      void read(Buffer& temp);

      //These could be enums that got converted to BYTEs in the constructor.

      //for reference see Part 8/table 9-21

      //Result
      const static std::uint8_t REJECTED_PERMANENT = 1;
      const static std::uint8_t REJECTED_TRANSIENT = 2;
      //source
      const static std::uint8_t DICOM_SERVICE_USER = 1;
      const static std::uint8_t DICOM_SERVICE_PROVIDER_ACSE = 2;
      const static std::uint8_t DICOM_SERVICE_PROVIDER_PRESENTATION = 3;
      //reason
      const static std::uint8_t RESERVED = 0;
      //source has value 1
      const static std::uint8_t NO_REASON = 1;
      const static std::uint8_t APPLICATION_CONTEXT_NOT_SUPPORTED = 2;
      const static std::uint8_t CALLING_AE_NOT_RECOGNIZED = 3;
      const static std::uint8_t CALLED_AE_NOT_RECOGNIZED = 7;
      //source has value 2
      const static std::uint8_t PROTOCOL_VERSION_NOT_SUPPORTED = 2;
      //source has value 3
      const static std::uint8_t TEMPORARY_CONGESTION = 1;
      const static std::uint8_t LOCAL_LIMIT_EXCEEDED = 2;
    };

    /*!
    Defined in Part 8, table 9-24
    */
    class AReleaseRQ
    {
      const static std::uint8_t m_itemType = 0x05;
      const static std::uint8_t m_reserved1 = 0x00;
      const static std::uint32_t m_length = 0x04;
      const static std::uint32_t m_reserved2 = 0x00;

    public:

      void write(Buffer& temp);
      void read(Buffer& temp);
    };

    /*!
    Defined in Part8, table 9-25
    */
    class AReleaseRP
    {
      const static std::uint8_t m_itemType = 0x06;
      const static std::uint8_t m_reserved1 = 0x00;
      const static std::uint32_t m_length = 0x04;
      const static std::uint32_t m_reserved2 = 0x00;

    public:

      void write(Buffer& temp);
      void read(Buffer& temp);
    };

    /*!
    Defined in Part 8, table 9-26
    Should be called just 'AAbort'
    */
    class AAbortRQ
    {
      const static std::uint8_t m_itemType = 0x07;
      const static std::uint8_t m_reserved1 = 0x00;
      const static std::uint32_t m_length = 0x04;
      const static std::uint8_t m_reserved2 = 0x00;
      const static std::uint8_t m_reserved3 = 0x00;

    public:

      std::uint8_t m_source;
      std::uint8_t m_reason;

      AAbortRQ(std::uint8_t Source, std::uint8_t Reason);

      void write(Buffer& temp);
      void read(Buffer& temp);

      //source (who  initiated the abort.)
      const static std::uint8_t DICOM_SERVICE_USER = 0;
      const static std::uint8_t DICOM_SERVICE_PROVIDER = 2;
      //reason (should be 0 if Source is 0)
      const static std::uint8_t NO_REASON = 0;
      const static std::uint8_t UNRECOGNIZED_PDU = 1;
      const static std::uint8_t UNEXPECTED_PDU = 2;
      const static std::uint8_t UNRECOGNIZED_PDU_PARAMETER = 4;
      const static std::uint8_t UNEXPECTED_PDU_PARAMETER = 5;
      const static std::uint8_t INVALID_PDU_PARAMETER = 6;
    };
  }//namespace primitive
}//namespace dicom
