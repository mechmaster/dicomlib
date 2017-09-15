/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "aarj.hpp"
#include "aarq.hpp"//for BadItemType

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
    const std::uint8_t AAssociateRJ::m_itemType;
    const std::uint8_t AAssociateRJ::m_reserved1;
    const std::uint32_t AAssociateRJ::m_length;
    const std::uint8_t AAssociateRJ::m_reserved2;

    const std::uint8_t AReleaseRQ::m_itemType;
    const std::uint8_t AReleaseRQ::m_reserved1;
    const std::uint32_t AReleaseRQ::m_length;
    const std::uint32_t AReleaseRQ::m_reserved2;

    const std::uint8_t AReleaseRP::m_itemType;
    const std::uint8_t AReleaseRP::m_reserved1;
    const std::uint32_t AReleaseRP::m_length;
    const std::uint32_t AReleaseRP::m_reserved2;

    const std::uint8_t AAbortRQ::m_itemType;
    const std::uint8_t AAbortRQ::m_reserved1;
    const std::uint32_t AAbortRQ::m_length;
    const std::uint8_t AAbortRQ::m_reserved2;
    const std::uint8_t AAbortRQ::m_reserved3;
#endif

    AAssociateRJ::AAssociateRJ(std::uint8_t Result, std::uint8_t Source, std::uint8_t Reason) :
      m_result(Result),
      m_source(Source),
      m_reason(Reason)
    {
    }

    AAssociateRJ::AAssociateRJ()
    {
    }


    void AAssociateRJ::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;
      temp << m_length;
      temp << m_reserved2;
      temp << m_result;
      temp << m_source;
      temp << m_reason;
    }

    void AAssociateRJ::read(Buffer& temp)
    {
      std::uint8_t b;
      std::uint32_t bbbb;
      
      temp >> b;
      EnforceItemType(b, m_itemType);
      
      temp >> b; // m_reserved1;
      temp >> bbbb; // m_length;
      temp >> b; // m_reserved2;
      temp >> m_result;
      temp >> m_source;
      temp >> m_reason;
    }

    /************************************************************************
    *
    * AReleaseRQ Packet
    *
    ************************************************************************/

    void AReleaseRQ::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;
      temp << m_length;
      temp << m_reserved2;
    }

    /*!
    Section 8, table 9.3.7
    */
    void AReleaseRQ::read(Buffer& temp)
    {
      std::uint8_t b;
      std::uint32_t bbbb;
      
      temp >> b;
      EnforceItemType(b, m_itemType);
      
      temp >> b;
      temp >> bbbb;//should be 4 but we don't bother checking.
      temp >> bbbb;
    }

    /************************************************************************
    *
    * AReleaseRP Packet
    *
    ************************************************************************/

    void AReleaseRP::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;
      temp << m_length;
      temp << m_reserved2;
    }

    void AReleaseRP::read(Buffer& temp)
    {
      std::uint8_t b;
      std::uint32_t bbbb;
      
      temp >> b;
      EnforceItemType(b, m_itemType);

      temp >> b;
      temp >> bbbb;
      temp >> bbbb;
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

    AAbortRQ::AAbortRQ(std::uint8_t Source, std::uint8_t Reason) :
      m_source(Source),
      m_reason(Reason)
    {
    }

    void AAbortRQ::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;
      temp << m_length;
      temp << m_reserved2;
      temp << m_reserved3;
      temp << m_source;
      temp << m_reason;
    }

    void AAbortRQ::read(Buffer& temp)
    {
      std::uint8_t b;
      std::uint32_t bbbb;
      
      temp >> b;
      EnforceItemType(b, m_itemType);

      temp >> b;
      temp >> bbbb;
      temp >> b;
      temp >> b;
      temp >> m_source;
      temp >> m_reason;
    }
  }//namespace primitive
}//namespace dicom
