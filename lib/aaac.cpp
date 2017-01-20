/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <algorithm>
#include <functional>

#include "aaac.hpp"

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
    const BYTE PresentationContextAccept::m_itemType;
    const BYTE PresentationContextAccept::m_reserved1;
    const BYTE PresentationContextAccept::m_reserved2;
    const BYTE PresentationContextAccept::m_reserved4;

    const BYTE AAssociateAC::m_itemType;
    const BYTE AAssociateAC::m_reserved1;
    const UINT16 AAssociateAC::m_protocolVersion;
    const UINT16 AAssociateAC::m_reserved2;
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

    PresentationContextAccept::PresentationContextAccept() :
      m_trnSyntax(UID(""))
    {
    }

    void PresentationContextAccept::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved1;
      socket << UINT16(size() - 4);
      socket << m_presentationContextID;
      socket << m_reserved2;
      socket << m_result;
      socket << m_reserved4;
      m_trnSyntax.Write(socket);
    }

    UINT32 PresentationContextAccept::read(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      BYTE b;
      socket >> b;
      byteread += sizeof(b);
      EnforceItemType(b, m_itemType);

      byteread += readDynamic(socket);
      return byteread;
    }

    /*!
    See Section 8, table 9-18 for desciption of the following fields.

    */
    UINT32 PresentationContextAccept::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      socket >> tmpBYTE; //Reserved
      byteread += sizeof(tmpBYTE);
      UINT16 length;
      socket >> length; //number of bytes from here to the end of the TransferSyntax item
      byteread += sizeof(length);
      //As far as I can see this is redundant apart from as an error check.
      socket >> m_presentationContextID;
      socket >> tmpBYTE; //Reserved
      socket >> m_result; //between 0 and 4
      socket >> tmpBYTE; //Reserved
      byteread += sizeof(m_presentationContextID) + sizeof(tmpBYTE) + sizeof(m_result) + sizeof(tmpBYTE);

      byteread += m_trnSyntax.Read(socket);

      Enforce(size() - 4 == length);
      return byteread;
    }

    UINT16 PresentationContextAccept::size()
    {
      return m_trnSyntax.Size() + 8;
    }

    /************************************************************************
    *
    * AAssociateAC Packet
    *
    ************************************************************************/

    AAssociateAC::AAssociateAC() :
      m_appContext(UID(""))
    {
      std::fill(m_reserved3.begin(), m_reserved3.end(), 0);
    }

    AAssociateAC::AAssociateAC(const std::string& callingApp, const std::string& calledApp) :
      m_calledAppTitle(calledApp),
      m_callingAppTitle(callingApp),
      m_appContext(UID(""))//not at all happy about this...
    {
      std::fill(m_reserved3.begin(), m_reserved3.end(), 0);
    }

    //!I feel this should be happening in the constructor...

    void AAssociateAC::setUserInformation(UserInformation& user)
    {
      m_userInfo = user;
    }

    void AAssociateAC::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved1;

      socket << UINT32(size() - 6);

      socket << m_protocolVersion;
      socket << m_reserved2;

      socket.Sendn<char>(&m_calledAppTitle[0], m_calledAppTitle.size());
      socket.Sendn(&m_callingAppTitle[0], m_callingAppTitle.size());

      socket.Sendn<BYTE>(&m_reserved3[0], m_reserved3.size());
      m_appContext.Write(socket);

      std::for_each(m_presContextAccepts.begin(),m_presContextAccepts.end(), WriteToSocket(socket));

      m_userInfo.Write(socket);
    }

    UINT32 AAssociateAC::read(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      BYTE b;
      socket >> b;
      byteread += sizeof(b);

      EnforceItemType(b, m_itemType);

      byteread += readDynamic(socket);
      return byteread;
    }

    UINT32 AAssociateAC::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread=0;
      UINT32 tmp_read=0;

      BYTE TempByte;

      socket >> tmpBYTE; //Reserved1_;
      byteread += sizeof(tmpBYTE);

      UINT32 length;
      socket >> length;

      socket >> tmpUINT16;
      /*
      verify that (tmpUINT16 bitand 0x01) is true; - see part 8, table 9-17
      */

      socket >> tmpUINT16;

      byteread += sizeof(length) + 2 * sizeof(tmpUINT16);

      m_calledAppTitle.assign(16, ' ');
      socket.Read(m_calledAppTitle);
      byteread += 16;

      StripTrailingWhitespace(m_calledAppTitle);

      m_callingAppTitle.assign(16,' ');
      socket.Read(m_callingAppTitle);
      byteread += 16;

      StripTrailingWhitespace(m_callingAppTitle);

      socket.Readn<BYTE>(&m_reserved3[0], 32);
      byteread += 32;

      int BytesLeftToRead = length - sizeof(UINT16) - sizeof(UINT16) - 64;
      while(BytesLeftToRead > 0)
      {
        socket >> TempByte;

        BytesLeftToRead -= sizeof(TempByte);
        byteread += sizeof(TempByte);

        switch(TempByte)
        {
          case 0x50: // user information
            tmp_read = m_userInfo.ReadDynamic(socket);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read;//UserInfo_.Size();
          break;
          case 0x21:
          {
            PresentationContextAccept PresContextAccept;//should be inside loop.
            tmp_read = PresContextAccept.readDynamic(socket);
            BytesLeftToRead -= tmp_read;//PresContextAccept.Size();
            byteread += tmp_read;
            m_presContextAccepts.push_back(PresContextAccept);
          }
          break;
          case 0x10:
            tmp_read = m_appContext.ReadDynamic(socket);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read;//AppContext_.Size();
          break;
          default:
            throw BadItemType(TempByte, 0);
        }
      }

      //This line is not enforcible because it may not be true-Sam 28Nov2007
      //Enforce(length==Size()-6);

      //This if will never happen because of the while loop above. -Sam 28Nov2007
      //if(BytesLeftToRead)
      //throw dicom::exception("non-zero remaining bytes in AAssociateAC::ReadDynamic()");
      return byteread;
    }

    UINT32 AAssociateAC::size()
    {
      UINT32 length = sizeof(UINT16) + sizeof(UINT16) + 64;
      length += m_appContext.Size();

      std::vector<PresentationContextAccept>::iterator iter = m_presContextAccepts.begin();
      for (; iter != m_presContextAccepts.end(); ++iter)
      {
        length += iter->size();
      }

      length += m_userInfo.Size();
      return (length + sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT32));
    }
  }// namespace primitive
}// namespace dicom
