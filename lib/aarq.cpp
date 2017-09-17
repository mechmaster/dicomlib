/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/
#include <algorithm>
#include <functional>

#include "aarq.hpp"
#include "UIDs.hpp"
#include "Exceptions.hpp"

/*IMPL_VERSION_NAME
There is much repeated functionality in this file,
implying a need for a shared base class.

Some of the size members can be consolidated?
*/

/*
We need a clearer approach to managing message sizes.  Generally speaking,
the Size() member function does two things.  It sets the Length_ member
and returns a value.  Generally Size() is the entire byte-size of the message,
and Length_ is the size of the message minus the initial couple of fields.
(The initial fields generally being 'item type', 'reserved' and 'length'

So Length_ corresponds to the Length_ field, and Size() to the entire
message Size(). 

There are two problems here from a coding perspective.  One is that the 
Size() function has non-obvious side effects, and the other is that we 
seem to be duplicating information, which is asking for bugs, as has
already occured a few times.  My proposal is to lose Length_ completely
as a member, and when it is needed use Size()-(size of initial fields).

I think this would increase data integrity.


IMPORTANT!  Are all Size() fields UINT16 ????
NO THEY ARE NOT!  EXTRA CAUTION REQUIRED!
*/


/*************************************************************************
*
* Application Context Class
*
************************************************************************/

namespace dicom
{
  void EnforceItemType(std::uint8_t Given, std::uint8_t Expected)
  {
    if(Expected != Given)
    {
      throw BadItemType(Given, Expected);
    }
  }

  namespace primitive
  {
    #ifndef _MSC_VER
    /*
    difference of opinion here between gcc and msvc as to whether constant static members
    need to be instantiated seperately.
    */
    const std::uint8_t PresentationContext::m_itemType;
    const std::uint8_t PresentationContext::m_reserved1;
    const std::uint8_t PresentationContext::m_reserved2;
    const std::uint8_t PresentationContext::m_reserved3;
    const std::uint8_t PresentationContext::m_reserved4;

    const std::uint8_t MaximumSubLength::m_itemType;
    const std::uint8_t MaximumSubLength::m_reserved1;
    const std::uint16_t MaximumSubLength::m_length;

    const std::uint8_t UserInformation::m_itemType;
    const std::uint8_t UserInformation::m_reserved;

    const std::uint8_t AAssociateRQ::m_itemType;
    const std::uint8_t AAssociateRQ::m_reserved1;
    const std::uint16_t AAssociateRQ::m_protocolVersion;
    const std::uint16_t AAssociateRQ::m_reserved2;

    const std::uint8_t ApplicationContext::m_itemType;
    const std::uint8_t ApplicationContext::m_reserved;

    const std::uint8_t AbstractSyntax::m_itemType;
    const std::uint8_t AbstractSyntax::m_reserved;

    const std::uint8_t TransferSyntax::m_itemType;
    const std::uint8_t TransferSyntax::m_reserved;

    const std::uint8_t ImplementationClass::m_itemType;
    const std::uint8_t ImplementationClass::m_reserved;

    const std::uint8_t ImplementationVersion::m_itemType;
    const std::uint8_t ImplementationVersion::m_reserved;

    const std::uint8_t SCPSCURoleSelect::m_itemType;
    const std::uint8_t SCPSCURoleSelect::m_reserved;
    #endif

    ApplicationContext::ApplicationContext(const UID& uid) :
      m_UID(uid)
    {
    }

    std::uint32_t ApplicationContext::size()
    {
      return (sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint16_t) + m_UID.str().size());
    }

    std::uint32_t ApplicationContext::read(Buffer& temp)
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

      std::string s(length, ' ');
      temp >> s;
      byteread += length;
      m_UID = UID(s);

      return byteread;
    }

    void ApplicationContext::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved;

      std::uint16_t length = std::uint16_t(m_UID.str().size());
      temp << length;

      temp << m_UID.str();
    }

    /*************************************************************************
    *
    * Abstract Syntax Class
    *
    ************************************************************************/

    AbstractSyntax::AbstractSyntax(const UID& uid) :
      m_UID(uid)
    {
    }

    std::uint32_t AbstractSyntax::size()
    {
      return (sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint16_t) + m_UID.str().size());
    }

    std::uint32_t AbstractSyntax::read(Buffer& temp)
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
      
      std::string s(length, ' ');
      temp >> s;
      byteread += length;
      m_UID = UID(s);
      
      return byteread;
    }

    void AbstractSyntax::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved;
      
      std::uint16_t length = std::uint16_t(m_UID.str().size());
      temp << length;
      
      temp << m_UID.str();
    }

    TransferSyntax::TransferSyntax(const UID& uid) :
      m_UID(uid)
    {
    }

    std::uint32_t TransferSyntax::size()
    {
      return (sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint16_t) + m_UID.str().size());
    }

    void TransferSyntax::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved;
      
      std::uint16_t length = std::uint16_t(m_UID.str().size());
      temp << length;
      
      temp << m_UID.str();
    }

    std::uint32_t TransferSyntax::read(Buffer& temp)
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
      
      std::string s(length, ' ');
      temp >> s;
      byteread += length;
      m_UID = UID(s);
      
      return byteread;
    }

    /************************************************************************
    *
    *  Implementation Class
    *
    ************************************************************************/

    ImplementationClass::ImplementationClass(const UID& uid) :
      m_UID(uid)
    {
    }

    std::uint32_t ImplementationClass::size()
    {
      return (sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint16_t) + m_UID.str().size());
    }
    
    std::uint32_t ImplementationClass::read(Buffer& temp)
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
      
      std::string s(length, ' ');
      temp >> s;
      byteread += length;
      m_UID = UID(s);
      
      return byteread;
    }

    void ImplementationClass::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved;
      
      std::uint16_t length = std::uint16_t(m_UID.str().size());
      temp << length;
      
      temp << m_UID.str();
    }

    /************************************************************************
    *
    *  Implementation Version
    *
    ************************************************************************/

    std::uint32_t ImplementationVersion::size()
    {
      if (m_name.empty())
      {
        return 0;//message doesn't get sent if it's empty.
      }

      return (sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint16_t) + m_name.size());
    }

    std::uint32_t ImplementationVersion::read(Buffer& temp)
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
      
      if (length > 16)
      {
        throw dicom::exception("Implementation Version Length too long.");
      }
      
      m_name.assign(length, ' ');
      temp >> m_name;
      byteread += length;
      
      return byteread;
    }

    void ImplementationVersion::write(Buffer& temp)
    {
      if(m_name.empty())//message doesn't get sent if it's empty.
      {
        return;
      }

      temp << m_itemType;
      temp << m_reserved;
      
      std::uint16_t length = std::uint16_t(m_name.size());
      temp << length;
      
      temp << m_name;
    }

    /************************************************************************
    *
    * Presentation Context
    *
    ************************************************************************/

    PresentationContext::PresentationContext(const AbstractSyntax& Abs,
      const std::vector<TransferSyntax>& AcceptableTransferSyntaxes, std::uint8_t id) :
      m_transferSyntaxes(AcceptableTransferSyntaxes),
      m_ID(id),
      m_AbsSyntax(Abs)
    {
    }

    /*!
    This form defaults to ImplicitVR/Little Endian
    */
    PresentationContext::PresentationContext(const AbstractSyntax& Abs, std::uint8_t id) :
      m_ID(id),
      m_AbsSyntax(Abs)
    {
      m_transferSyntaxes.push_back(TransferSyntax(IMPL_VR_LE_TRANSFER_SYNTAX));
    }

    PresentationContext::PresentationContext() :
      m_ID(0x00),
      m_AbsSyntax(UID(""))
    {
    }

    void PresentationContext::addTransferSyntax(TransferSyntax& Tran)
    {
      m_transferSyntaxes.push_back (Tran);
    }

    void writeTS(TransferSyntax transferSyntaxes, Buffer& temp)
    {
      transferSyntaxes.write(temp);
    }
    
    void PresentationContext::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;

      temp << m_length;
      temp << m_ID;
      temp << m_reserved2;
      temp << m_reserved3;
      temp << m_reserved4;

      m_AbsSyntax.write(temp);

      std::for_each(m_transferSyntaxes.begin(), m_transferSyntaxes.end(),
        std::bind(writeTS, std::placeholders::_1, std::ref(temp)));
    }

    std::uint32_t PresentationContext::read(Buffer& temp)
    {
      std::uint32_t byteread = 0;
      
      std::uint8_t b;
      temp >> b;
      byteread += sizeof(b);
      EnforceItemType(b, m_itemType);
      
      temp >> b;
      byteread += sizeof(b);
      temp >> m_length; //number of bytes from here to the end of the TransferSyntax item
      byteread += sizeof(m_length);
      
      temp >> m_ID;
      byteread += sizeof(m_ID);

      temp >> b;
      byteread += sizeof(b);
      temp >> b;
      byteread += sizeof(b);
      temp >> b;
      byteread += sizeof(b);

      std::uint32_t count = m_length - sizeof(std::uint8_t) - sizeof(std::uint8_t) - sizeof(std::uint8_t) - sizeof(std::uint8_t);

      std::uint32_t tmp_read = m_AbsSyntax.read(temp);
      byteread += tmp_read;
      count -= tmp_read; //AbsSyntax_.Size();

      TransferSyntax Tran(UID(""));
      //Read in the transfer syntaxes...
      while (count > 0)
      {
        tmp_read = Tran.read(temp);
        byteread += tmp_read;
        count -= tmp_read; //Tran.Size();

        m_transferSyntaxes.push_back(Tran);
      }

      Enforce(0 == count);

      return byteread;
    }

    std::uint32_t PresentationContext::size()
    {
      m_length = sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint8_t);
      m_length += m_AbsSyntax.size();

      size_t Index = 0;

      while (Index < m_transferSyntaxes.size())
      {
        m_length += m_transferSyntaxes.at(Index).size();
        ++Index;
      }

      return (m_length + sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint16_t));
    }

    /************************************************************************
    *
    * Maximum Sub Length
    *
    ************************************************************************/

    MaximumSubLength::MaximumSubLength()
    {
    }

    MaximumSubLength::MaximumSubLength(std::uint32_t Max)
    {
      m_maximumLength = Max;
    }

    void MaximumSubLength::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;
      temp << m_length;
      temp << m_maximumLength;
    }

    std::uint32_t MaximumSubLength::read(Buffer& temp)
    {
      std::uint32_t byteread = 0;
      
      std::uint8_t b;
      temp >> b;
      byteread += sizeof(b);
      EnforceItemType(b, m_itemType);
      
      temp >> b;
      byteread += sizeof(b);
      std::uint16_t length;
      temp >> length;
      byteread += sizeof(m_length);

      if (length != m_length)
      {
        throw dicom::exception("itemlength of MaximumSubLength must be 0x04");
      }

      temp >> m_maximumLength;
      byteread += sizeof(m_maximumLength);

      return byteread;
    }

    std::uint32_t MaximumSubLength::size()
    {
      return (m_length + sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t));
    }

    /******** SCP / SCU Role Select ***********/

    SCPSCURoleSelect::SCPSCURoleSelect() :
      m_UID("")
    {
    }

    void SCPSCURoleSelect::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved;
      temp << m_itemLength;
      temp << static_cast<std::uint16_t>(m_UID.str().size());
      temp << m_UID.str();

      temp << m_SCURole;
      temp << m_SCPRole;
    }

    std::uint32_t SCPSCURoleSelect::read(Buffer& temp)
    {
      std::uint32_t byteread = 0;
      
      std::uint8_t b;
      temp >> b;
      byteread += sizeof(b);
      EnforceItemType(b, m_itemType);
      
      temp >> b;
      byteread += sizeof(b);
      temp >> m_itemLength;
      byteread += sizeof(m_itemLength);
      
      std::uint16_t size = 0;
      temp >> size;
      byteread += sizeof(size);

      std::string s(size, ' ');
      temp >> s;
      byteread += size;
      m_UID = UID(s);

      temp >> m_SCURole;
      temp >> m_SCPRole;
      byteread += sizeof(m_SCURole) + sizeof(m_SCPRole);

      return byteread;
    }

    std::uint32_t SCPSCURoleSelect::size()
    {
      return (sizeof(std::uint8_t) +
        sizeof(std::uint8_t) +
        sizeof(std::uint16_t) +
        m_UID.str().size() +
        sizeof(std::uint8_t) +
        sizeof(std::uint8_t));
    }

    /************************************************************************
    *
    * User Information
    *
    ************************************************************************/

    UserInformation::UserInformation() :
      m_userInfoBaggage(0),
      m_impClass(UID(""))
    {
    }

    void UserInformation::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved;

      temp << std::uint32_t(size() - 6);

      m_maxSubLength.write(temp);
      m_impClass.write(temp);

      //should only send this if it really exists...
      m_impVersion.write(temp);

      /*
      Note that we don't currently support writing the 
      SCPSCURoleSelect Sub-item (which is optional.)
      */
    }

    std::uint32_t UserInformation::read(Buffer& temp)
    {
      std::uint32_t byteread = 0;
      
      std::uint8_t b;
      temp >> b;
      byteread += sizeof(b);
      EnforceItemType(b, m_itemType);

      temp >> b;
      byteread += sizeof(b);
      std::uint32_t length;
      temp >> length;
      byteread += sizeof(length);

      std::uint32_t BytesLeftToRead = length;

      while(BytesLeftToRead > 0)
      {
        temp >> b;
        BytesLeftToRead -= sizeof(b);
        byteread += sizeof(b);

        std::uint32_t tmp_read;
        switch (b)
        {
          case 0x51:
            tmp_read = m_maxSubLength.read(temp);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read; //MaxSubLength_.Size();
          break;
          case 0x52:
            tmp_read = m_impClass.read(temp);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read; //ImpClass_.Size();
          break;
          case 0x54: // Role selection
          /*
          This is very ugly, the use of UserInfoBaggage_ is not a nice idea.
          It would be better to do some clever trick in SCPSCURole to manage
          optionality, maybe.
          */
            tmp_read = m_SCPSCURole.read(temp);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read; //SCPSCURole_.Size();
            m_userInfoBaggage += m_SCPSCURole.size();
          break;
          case 0x55:
            tmp_read = m_impVersion.read(temp); //optional!
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read; //ImpVersion_.Size();
          break;
          default:
            throw BadItemType(b, 0);
        }
      }

      return byteread;
    }

    std::uint32_t UserInformation::size()
    {
      std::uint32_t length = m_maxSubLength.size();
      length += m_impClass.size();

      /*
      now ImpVersion_ and SCPSCURoleSelect are both optional.  This fact needs to be
      taken into account when sending and receiving.
      */

      length += m_impVersion.size();

      length += m_userInfoBaggage; //need to do this better.
      //problem is, SCP/SCU role is an optional sub-item

      return length + 4;
    }

    /************************************************************************
    *
    * AAssociateRQ Packet
    *
    ************************************************************************/

    AAssociateRQ::AAssociateRQ() :
      m_appContext(APPLICATION_CONTEXT)
    {
      std::fill(m_reserved3.begin(), m_reserved3.end(), 0);
    }

    AAssociateRQ::AAssociateRQ(const std::string& CallingAp, const std::string& CalledAp) :
      m_calledAppTitle(CalledAp),
      m_callingAppTitle(CallingAp),
      m_appContext(APPLICATION_CONTEXT)
    {
      std::fill(m_reserved3.begin(), m_reserved3.end(), 0);
    }

    //!This should be in the constructor I think
    void AAssociateRQ::setUserInformation(UserInformation& User)
    {
      m_userInfo = User;
    }

    void AAssociateRQ::write(Buffer& temp)
    {
      temp << m_itemType;
      temp << m_reserved1;

      temp << std::uint32_t(size() - 6); //Length_;

      temp << m_protocolVersion;
      temp << m_reserved2;

      temp << m_calledAppTitle;
      temp << m_callingAppTitle;

      std::for_each(m_reserved3.begin(), m_reserved3.end(), std::bind(SendByte, std::placeholders::_1, std::ref(temp)));

      m_appContext.write(temp);

      std::vector<PresentationContext>::iterator iter = m_proposedPresentationContexts.begin();
      for (; iter != m_proposedPresentationContexts.end(); ++iter)
      {
        iter->write(temp);
      }

      m_userInfo.write(temp);
    }

    std::uint32_t AAssociateRQ::read(Buffer& temp)
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
      
      std::uint32_t BytesLeftToRead = length - sizeof(std::uint16_t) - sizeof(std::uint16_t) - 64;
      while(BytesLeftToRead > 0)
      {
        temp >> b;
        BytesLeftToRead -= sizeof(b);
        byteread += sizeof(b);

        std::uint32_t tmp_read;
        switch(b)
        {
          case 0x50: // user information
            tmp_read = m_userInfo.read(temp);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read; //UserInfo_.Size(); // - UserInfo_.UserInfoBaggage;
            break;
          case 0x20:
          {
            PresentationContext PresContext; //I'd rather these two lines were compacted to one 
            tmp_read = PresContext.read(temp); //using a constructor.
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read; //PresContext.Size();
            m_proposedPresentationContexts.push_back(PresContext);
          }
          break;
          case 0x10:
          {
            ApplicationContext app_context(APPLICATION_CONTEXT);
            tmp_read = app_context.read(temp);
            byteread += tmp_read;
            BytesLeftToRead -= tmp_read; //app_context.Size();
            if (app_context.m_UID != APPLICATION_CONTEXT)
            {
              throw exception("Unsupported Application Context");//this should be throw AssociationException(...)
            }
          }
          break;
          default:
            throw BadItemType(b, 0);
        }
      }
      
      if(BytesLeftToRead)
      {
        throw dicom::exception("BytesLeftToRead != 0 at end of aarq read." );
      }
      
      return byteread;
    }

    std::uint32_t AAssociateRQ::size()
    {
      std::uint32_t length = sizeof(std::uint16_t) + sizeof(std::uint16_t) + 64;
      length += m_appContext.size();
      size_t Index = 0;

      while(Index < m_proposedPresentationContexts.size())
      {
        length += m_proposedPresentationContexts[Index].size();
        ++Index;
      }

      length += m_userInfo.size();
      return (length + sizeof(std::uint8_t) + sizeof(std::uint8_t) + sizeof(std::uint32_t));
    }
  }//namespace primitive
}//namespace dicom
