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
    const std::uint8_t PresentationContextAccept::m_itemType;
    const std::uint8_t PresentationContextAccept::m_reserved1;
    const std::uint8_t PresentationContextAccept::m_reserved2;
    const std::uint8_t PresentationContextAccept::m_reserved4;

    const std::uint8_t AAssociateAC::m_itemType;
    const std::uint8_t AAssociateAC::m_reserved1;
    const std::uint16_t AAssociateAC::m_protocolVersion;
    const std::uint16_t AAssociateAC::m_reserved2;
#endif

    PresentationContextAccept::PresentationContextAccept() :
      m_trnSyntax(UID(""))
    {
    }

    void PresentationContextAccept::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;
      temp << std::uint16_t(size() - 4);
      temp << m_presentationContextID;
      temp << m_reserved2;
      temp << m_result;
      temp << m_reserved4;

      m_trnSyntax.write(temp);
    }

    std::uint32_t PresentationContextAccept::read(Buffer& temp)
    {
      std::uint32_t byteread = 0;

      std::uint8_t b;
      temp >> b;
      byteread += sizeof(b);
      EnforceItemType(b, m_itemType);

      temp >> b; //Reserved
      byteread += sizeof(b);
      std::uint16_t length;
      temp >> length; //number of bytes from here to the end of the TransferSyntax item
      byteread += sizeof(length);
      //As far as I can see this is redundant apart from as an error check.
      temp >> m_presentationContextID;
      temp >> b; //Reserved
      temp >> m_result; //between 0 and 4
      temp >> b; //Reserved
      byteread += sizeof(m_presentationContextID) + sizeof(b) + sizeof(m_result) + sizeof(b);

      byteread += m_trnSyntax.read(temp);
      
      Enforce(size() - 4 == length);

      return byteread;
    }

    std::uint16_t PresentationContextAccept::size()
    {
      return m_trnSyntax.size() + 8;
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

    void AAssociateAC::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;

      temp << std::uint32_t(size() - 6);

      temp << m_protocolVersion;
      temp << m_reserved2;

      temp << m_calledAppTitle;
      temp << m_callingAppTitle;

      std::for_each(m_reserved3.begin(), m_reserved3.end(), std::bind(SendByte, std::placeholders::_1, std::ref(temp)));
      
      m_appContext.write(temp);

      std::vector<PresentationContextAccept>::iterator iter = m_presContextAccepts.begin();
      for (; iter != m_presContextAccepts.end(); ++iter)
      {
        iter->write(temp);
      }

      m_userInfo.write(temp);
    }

    std::uint32_t AAssociateAC::read(Buffer& temp)
    {
      std::uint32_t byteread = 0;
      std::uint8_t b;
      std::uint16_t bb;

      temp >> b;
      byteread += sizeof(b);

      EnforceItemType(b, m_itemType);
      
      temp >> b; //Reserved1_;
      byteread += sizeof(b);
      
      std::uint32_t length;
      temp >> length;
      
      temp >> bb;
      /*
       v *erify that (tmpUINT16 bitand 0x01) is true; - see part 8, table 9-17
       */
      
      temp >> bb;
      
      byteread += sizeof(length) + 2 * sizeof(std::uint16_t);
      
      m_calledAppTitle.assign(16, ' ');
      temp >> m_calledAppTitle;
      byteread += 16;
      
      StripTrailingWhitespace(m_calledAppTitle);
      
      m_callingAppTitle.assign(16,' ');
      temp >> m_callingAppTitle;
      byteread += 16;
      
      StripTrailingWhitespace(m_callingAppTitle);

      std::for_each(m_reserved3.begin(), m_reserved3.end(), std::bind(RecvByte, std::placeholders::_1, std::ref(temp)));
      byteread += 32;
      
      int BytesLeftToRead = length - sizeof(std::uint16_t) - sizeof(std::uint16_t) - 64;

      while(BytesLeftToRead > 0)
      {
        temp >> b;
        
        BytesLeftToRead -= sizeof(b);
        byteread += sizeof(b);
        
        switch(b)
        {
          case 0x50: // user information
          {
            std::uint32_t tmp_read = m_userInfo.read(temp);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read;//UserInfo_.Size();
          }
          break;
          case 0x21:
          {
            PresentationContextAccept PresContextAccept;//should be inside loop.
            std::uint32_t tmp_read = PresContextAccept.read(temp);
            BytesLeftToRead -= tmp_read;//PresContextAccept.Size();
            byteread += tmp_read;
            m_presContextAccepts.push_back(PresContextAccept);
          }
          break;
          case 0x10:
          {
            std::uint32_t tmp_read = m_appContext.read(temp);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read;//AppContext_.Size();
          }
          break;
          default:
            throw BadItemType(b, 0);
        }
      }

      return byteread;
    }

    std::uint32_t AAssociateAC::size()
    {
      std::uint32_t length = sizeof(std::uint16_t) + sizeof(std::uint16_t) + 64;
      length += m_appContext.size();

      std::vector<PresentationContextAccept>::iterator iter = m_presContextAccepts.begin();
      for (; iter != m_presContextAccepts.end(); ++iter)
      {
        length += iter->size();
      }

      length += m_userInfo.size();
      return (length + sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t));
    }
  }// namespace primitive
}// namespace dicom
