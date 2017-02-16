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
    const BYTE AAssociateRJ::m_itemType;
    const BYTE AAssociateRJ::m_reserved1;
    const UINT32 AAssociateRJ::m_length;
    const BYTE AAssociateRJ::m_reserved2;

    const BYTE AReleaseRQ::m_itemType;
    const BYTE AReleaseRQ::m_reserved1;
    const UINT32 AReleaseRQ::m_length;
    const UINT32 AReleaseRQ::m_reserved2;

    const BYTE AReleaseRP::m_itemType;
    const BYTE AReleaseRP::m_reserved1;
    const UINT32 AReleaseRP::m_length;
    const UINT32 AReleaseRP::m_reserved2;

    const BYTE AAbortRQ::m_itemType;
    const BYTE AAbortRQ::m_reserved1;
    const UINT32 AAbortRQ::m_length;
    const BYTE AAbortRQ::m_reserved2;
    const BYTE AAbortRQ::m_reserved3;
#endif

    namespace
    {
      BYTE tmpBYTE;
      UINT32 tmpUINT32;
    }

    AAssociateRJ::AAssociateRJ(BYTE Result, BYTE Source, BYTE Reason) :
      m_result(Result),
      m_source(Source),
      m_reason(Reason)
    {
    }

    AAssociateRJ::AAssociateRJ()
    {
    }


    void AAssociateRJ::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved1;
      socket << m_length;
      socket << m_reserved2;
      socket << m_result;
      socket << m_source;
      socket << m_reason;
    }

    void AAssociateRJ::readDynamic(Network::Socket& socket)
    {
      socket >> tmpBYTE; // m_itemType;
      socket >> tmpBYTE; // m_reserved1;
      socket >> tmpUINT32; // m_length;
      socket >> tmpBYTE; // m_reserved2;
      socket >> m_result;
      socket >> m_source;
      socket >> m_reason;
    }

    /************************************************************************
    *
    * AReleaseRQ Packet
    *
    ************************************************************************/

    void AReleaseRQ::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved1;
      socket << m_length;
      socket << m_reserved2;
    }

    /*!
    Section 8, table 9.3.7
    */
    void AReleaseRQ::readDynamic(Network::Socket& socket)
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

    void AReleaseRP::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved1;
      socket << m_length;
      socket << m_reserved2;
    }

    void AReleaseRP::read(Network::Socket& socket)
    {
      BYTE b;
      socket >> b;
      EnforceItemType(b, m_itemType);
      readDynamic(socket);
    }

    void AReleaseRP::readDynamic(Network::Socket& socket)
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
      readDynamic(socket);
    }

    AAbortRQ::AAbortRQ(BYTE Source, BYTE Reason) :
      m_source(Source),
      m_reason(Reason)
    {
    }

    void AAbortRQ::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved1;
      socket << m_length;
      socket << m_reserved2;
      socket << m_reserved3;
      socket << m_source;
      socket << m_reason;
    }

    void AAbortRQ::readDynamic(Network::Socket& socket)
    {
      socket >> tmpBYTE;
      socket >> tmpUINT32;
      socket >> tmpBYTE;
      socket >> tmpBYTE;
      socket >> m_source;
      socket >> m_reason;
    }
  }//namespace primitive
}//namespace dicom
