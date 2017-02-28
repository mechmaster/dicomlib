/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/
#include <algorithm>

#include "aarq.hpp"
#include "UIDs.hpp"
#include "Exceptions.hpp"
using std::for_each;

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
  void EnforceItemType(BYTE Given, BYTE Expected)
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
    const BYTE PresentationContext::m_itemType;
    const BYTE PresentationContext::m_reserved1;
    const BYTE PresentationContext::m_reserved2;
    const BYTE PresentationContext::m_reserved3;
    const BYTE PresentationContext::m_reserved4;

    const BYTE MaximumSubLength::m_itemType;
    const BYTE MaximumSubLength::m_reserved1;
    const UINT16 MaximumSubLength::m_length;

    const BYTE UserInformation::m_itemType;
    const BYTE UserInformation::m_reserved;

    const BYTE AAssociateRQ::m_itemType;
    const BYTE AAssociateRQ::m_reserved1;
    const UINT16 AAssociateRQ::m_protocolVersion;
    const UINT16 AAssociateRQ::m_reserved2;

    const BYTE ApplicationContext::m_itemType;
    const BYTE ApplicationContext::m_reserved;

    const BYTE AbstractSyntax::m_itemType;
    const BYTE AbstractSyntax::m_reserved;

    const BYTE TransferSyntax::m_itemType;
    const BYTE TransferSyntax::m_reserved;

    const BYTE ImplementationClass::m_itemType;
    const BYTE ImplementationClass::m_reserved;

    const BYTE ImplementationVersion::m_itemType;
    const BYTE ImplementationVersion::m_reserved;

    const BYTE SCPSCURoleSelect::m_itemType;
    const BYTE SCPSCURoleSelect::m_reserved;
    #endif

    /*
    CRIMINALLY DANGEROUS CODE AHEAD! PLEASE FIX!
    */

    namespace
    {
      // these  aren't thread safe, which could lead to horrendous complications if
      // you ever trust the data in them.  Only use them for dummy data
      // that never gets read.

      BYTE tmpBYTE;
      UINT32 tmpUINT32;
      UINT16 tmpUINT16;
    }

    ApplicationContext::ApplicationContext(const UID& uid) :
      m_UID(uid)
    {
    }

    UINT32 ApplicationContext::size() const
    {
      return (sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16) + m_UID.str().size());
    }

    UINT32 ApplicationContext::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      socket >> tmpBYTE;
      UINT16 UIDLength;
      socket >> UIDLength;
      byteread += sizeof(tmpBYTE) + sizeof(UIDLength);

      std::string s(UIDLength, ' ');
      socket.Read(s);
      byteread += UIDLength;
      m_UID = UID(s);

      return byteread;
    }

    void ApplicationContext::write(Network::Socket& socket) const
    {
      socket << m_itemType;
      socket << m_reserved;
      UINT16 Length = UINT16(m_UID.str().size()); //--|
      socket << Length; //--|these two lines could be amalgamated

      socket.Send(m_UID.str());
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

    UINT32 AbstractSyntax::size()
    {
      return (sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16) + m_UID.str().size());
    }

    UINT32 AbstractSyntax::read(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      BYTE b;
      socket >> b;

      byteread += sizeof(b);
      EnforceItemType(b, m_itemType);

      byteread += readDynamic(socket);
      return byteread;
    }

    UINT32 AbstractSyntax::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      socket >> tmpBYTE;
      UINT16 Length;
      socket >> Length;
      byteread += sizeof(tmpBYTE) + sizeof(Length);

      std::string s(Length,' ');
      socket.Read(s);
      byteread += Length;
      m_UID = UID(s);

      return byteread;
    }

    void AbstractSyntax::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved;
      socket << UINT16(m_UID.str().size());
      socket.Send(m_UID.str());
    }

    TransferSyntax::TransferSyntax(const UID& uid) :
      m_UID(uid)
    {
    }

    UINT32 TransferSyntax::size()
    {
      return (sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16) + m_UID.str().size());
    }

    void TransferSyntax::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved;
      socket << UINT16(m_UID.str().size());
      socket.Send(m_UID.str());
    }

    UINT32 TransferSyntax::read(Network::Socket& socket)
    {
      UINT32 byteread = 0;

      socket >> tmpBYTE;
      socket >> tmpBYTE;
      byteread += sizeof(tmpBYTE) + sizeof(tmpBYTE);

      UINT16 Length;
      socket >> Length;
      byteread += sizeof(Length);

      std::string s(Length, ' ');
      socket.Read(s);
      byteread += Length;
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

    UINT32 ImplementationClass::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      socket >> tmpBYTE;

      UINT16 Length;
      socket >> Length;
      byteread += sizeof(tmpBYTE) + sizeof(Length);

      std::string s(Length, ' ');
      socket.Read(s);
      byteread += Length;
      m_UID = UID(s);

      return byteread;
    }

    UINT32 ImplementationClass::size()
    {
      return (sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16) + m_UID.str().size());
    }

    void ImplementationClass::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved;
      socket << UINT16(m_UID.str().size());
      socket.Send(m_UID.str());
    }

    /************************************************************************
    *
    *  Implementation Version
    *
    ************************************************************************/

    UINT32 ImplementationVersion::size()
    {
      if(m_name.empty())
      {
        return 0;//message doesn't get sent if it's empty.
      }

      return (sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16) + m_name.size());
    }

    UINT32 ImplementationVersion::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      socket >> tmpBYTE;

      UINT16 Length;
      socket >> Length;
      byteread += sizeof(tmpBYTE) + sizeof(Length);

      if(Length > 16)
      {
        throw dicom::exception("Implementation Version Length too long.");
      }

      m_name.assign(Length, ' ');
      socket.Read(m_name);
      byteread += Length;

      return byteread;
    }

    void ImplementationVersion::write(Network::Socket& socket)
    {
      if(m_name.empty())//message doesn't get sent if it's empty.
      {
        return;
      }

      socket << m_itemType;
      socket << m_reserved;
      socket << UINT16(m_name.size());
      socket.Send(m_name);
    }

    /************************************************************************
    *
    * Presentation Context
    *
    ************************************************************************/

    PresentationContext::PresentationContext(const AbstractSyntax& Abs, const std::vector<TransferSyntax>& AcceptableTransferSyntaxes, BYTE id) :
      m_transferSyntaxes(AcceptableTransferSyntaxes),
      m_ID(id),
      m_AbsSyntax(Abs)
    {
    }

    /*!
    This form defaults to ImplicitVR/Little Endian
    */
    PresentationContext::PresentationContext(const AbstractSyntax& Abs, BYTE id) :
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

    void PresentationContext::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved1;

      socket << m_length;
      socket << m_ID;
      socket << m_reserved2;
      socket << m_reserved3;
      socket << m_reserved4;
      m_AbsSyntax.write(socket);

      for_each(m_transferSyntaxes.begin(), m_transferSyntaxes.end(), WriteToSocket(socket));
    }

    UINT32 PresentationContext::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;

      TransferSyntax Tran(UID(""));
      socket >> tmpBYTE;
      socket >> m_length;
      socket >> m_ID;
      socket >> tmpBYTE;
      socket >> tmpBYTE;
      socket >> tmpBYTE;

      byteread += 4 * sizeof(tmpBYTE) + sizeof(m_length) + sizeof(m_ID);

      int Count = m_length - sizeof(BYTE) - sizeof(BYTE) - sizeof(BYTE) - sizeof(BYTE);

      UINT32 tmp_read = m_AbsSyntax.read(socket);
      byteread += tmp_read;
      Count = Count - tmp_read;//AbsSyntax_.Size();

      //Read in the transfer syntaxes...
      while ( Count > 0)
      {
        tmp_read = Tran.read(socket);
        byteread += tmp_read;
        Count = Count - tmp_read;//Tran.Size();

        m_transferSyntaxes.push_back(Tran);
      }

      Enforce(0 == Count);

      return byteread;
    }

    UINT32 PresentationContext::size()
    {
      m_length = sizeof(BYTE) + sizeof(BYTE) + sizeof(BYTE) + sizeof(BYTE);
      m_length += m_AbsSyntax.size();

      size_t Index = 0;

      while (Index < m_transferSyntaxes.size())
      {
        m_length += m_transferSyntaxes.at(Index).size();
        ++Index;
      }

      return (m_length + sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16));
    }

    /************************************************************************
    *
    * Maximum Sub Length
    *
    ************************************************************************/

    MaximumSubLength::MaximumSubLength()
    {
    }

    MaximumSubLength::MaximumSubLength(UINT32 Max)
    {
      m_maximumLength = Max;
    }

    void MaximumSubLength::set(UINT32 Max)
    {
      m_maximumLength = Max;
    }

    UINT32 MaximumSubLength::get()
    {
      return (m_maximumLength);
    }

    void MaximumSubLength::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved1;
      socket << m_length;
      socket << m_maximumLength;
    }

    UINT32 MaximumSubLength::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;

      socket >> tmpBYTE;
      UINT16 Length;
      socket >> Length;
      byteread += sizeof(tmpBYTE) + sizeof(Length);

      if (Length != m_length)
      {
        throw dicom::exception("itemlength of MaximumSubLength must be 0x04");
      }

      socket >> m_maximumLength;
      byteread += sizeof(m_maximumLength);

      return byteread;
    }

    UINT32 MaximumSubLength::size()
    {
      return (m_length + sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16));
    }

    /******** SCP / SCU Role Select ***********/

    SCPSCURoleSelect::SCPSCURoleSelect() :
      m_UID("")
    {
    }

    void SCPSCURoleSelect::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved;
      socket << m_itemLength;
      socket << static_cast<UINT16>(m_UID.str().size());
      socket.Send(m_UID.str());

      socket << m_SCURole;
      socket << m_SCPRole;
    }

    UINT32 SCPSCURoleSelect::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;

      socket >> tmpBYTE;
      socket >> m_itemLength;
      byteread += sizeof(tmpBYTE) + sizeof(m_itemLength);

      UINT16 UIDLength;
      socket >> UIDLength;
      byteread += sizeof(UIDLength);

      std::string s(UIDLength, ' ');
      socket.Read(s);
      byteread += UIDLength;
      m_UID = UID(s);

      socket >> m_SCURole;
      socket >> m_SCPRole;
      byteread += sizeof(m_SCURole) + sizeof(m_SCPRole);

      return byteread;
    }


    //this should be fixed too  FIXME
    UINT32 SCPSCURoleSelect::size()
    {
      m_itemLength = sizeof(UINT16) + m_UID.str().size() + sizeof(BYTE) + sizeof(BYTE);
      return (m_itemLength + sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT16));
    }

    /************************************************************************
    *
    * User Information
    *
    ************************************************************************/

    UserInformation::UserInformation() :
      m_impClass(UID("")),
      m_userInfoBaggage(0)
    {
    }

    //do we ever use this?
    void UserInformation::setMax(MaximumSubLength& Max)
    {
      m_maxSubLength = Max;
    }

    void UserInformation::write(Network::Socket& socket)
    {
      socket << m_itemType;
      socket << m_reserved;

      socket << UINT16(size() - 4);
      //socket << Length_;//I think this should be a call to Size()

      m_maxSubLength.write(socket);
      m_impClass.write(socket);

      //should only send this if it really exists...
      m_impVersion.write(socket);

      /*
      Note that we don't currently support writing the 
      SCPSCURoleSelect Sub-item (which is optional.)
      */
    }

    UINT32 UserInformation::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      UINT32 tmp_read = 0;
      m_userInfoBaggage = 0;

      UINT16 length;
      socket >> tmpBYTE;
      socket >> length;
      byteread += sizeof(tmpBYTE) + sizeof(length);
      int BytesLeftToRead = length;

      while(BytesLeftToRead > 0)
      {
        socket >> tmpBYTE;
        BytesLeftToRead -= sizeof(tmpBYTE);
        byteread += sizeof(tmpBYTE);

        switch (tmpBYTE)
        {
          case 0x51:
            tmp_read = m_maxSubLength.readDynamic(socket);
            byteread += tmp_read;
            BytesLeftToRead = BytesLeftToRead - tmp_read;//MaxSubLength_.Size();
          break;
          case 0x52:
            tmp_read = m_impClass.readDynamic(socket);
            byteread += tmp_read;
            BytesLeftToRead = BytesLeftToRead - tmp_read;/*ImpClass_.Size()*/;
          break;
          case 0x54: // Role selection
          /*
          This is very ugly, the use of UserInfoBaggage_ is not a nice idea.
          It would be better to do some clever trick in SCPSCURole to manage
          optionality, maybe.
          */
            tmp_read = m_SCPSCURole.readDynamic(socket);
            byteread += tmp_read;
            BytesLeftToRead = BytesLeftToRead - tmp_read;//SCPSCURole_.Size();
            m_userInfoBaggage += m_SCPSCURole.size();
          break;
          case 0x55:
            tmp_read = m_impVersion.readDynamic(socket);//optional!
            byteread += tmp_read;
            BytesLeftToRead = BytesLeftToRead - tmp_read;//ImpVersion_.Size();
          break;
          default:
            throw BadItemType(tmpBYTE, 0);
        }
      }

      return byteread;
    }

    UINT16 UserInformation::size()
    {
      UINT16 length = m_maxSubLength.size();
      length += m_impClass.size();

      /*
      now ImpVersion_ and SCPSCURoleSelect are both optional.  This fact needs to be
      taken into account when sending and receiving.
      */

      length += m_impVersion.size();

      length += m_userInfoBaggage;//need to do this better.
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

    void AAssociateRQ::write(Network::Socket& socket)
    {
      size();
      socket << m_itemType;
      socket << m_reserved1;

      socket << UINT32(size() - 6);//Length_;

      socket << m_protocolVersion;
      socket << m_reserved2;

      socket.Sendn<char>(&m_calledAppTitle[0], m_calledAppTitle.size());
      socket.Sendn(&m_callingAppTitle[0], m_callingAppTitle.size());
      
      socket.Sendn<BYTE>(&m_reserved3[0], m_reserved3.size());

      m_appContext.write(socket);

      for_each(m_proposedPresentationContexts.begin(), m_proposedPresentationContexts.end(), WriteToSocket(socket));

      m_userInfo.write(socket);
    }

    UINT32 AAssociateRQ::read(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      BYTE b;
      socket >> b;
      byteread += sizeof(b);
      EnforceItemType(b, m_itemType);

      byteread += readDynamic(socket);
      return byteread;
    }

    UINT32 AAssociateRQ::readDynamic(Network::Socket& socket)
    {
      UINT32 byteread = 0;
      UINT32 tmp_read = 0;
      BYTE TempByte;
      socket >> tmpBYTE;

      UINT32 length;
      socket >> length;

      socket >> tmpUINT16;
      //check protocol version...
      socket >>	tmpUINT16;
      byteread += sizeof(tmpBYTE) + sizeof(length) + 2 * sizeof(tmpUINT16);

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

      int BytesLeftToRead = length - sizeof(UINT16) - sizeof(UINT16) - 16 - 16 - 32;
      while(BytesLeftToRead > 0)
      {
        socket >> TempByte;
        BytesLeftToRead -= sizeof(TempByte);
        byteread += sizeof(TempByte);

        switch(TempByte)
        {
          case 0x50: // user information
            tmp_read = m_userInfo.readDynamic(socket);
            byteread += tmp_read;
            BytesLeftToRead = BytesLeftToRead - tmp_read;//UserInfo_.Size();// - UserInfo_.UserInfoBaggage;
          break;
          case 0x20:
            {
              PresentationContext PresContext;//I'd rather these two lines were compacted to one 
              tmp_read = PresContext.readDynamic(socket);//using a constructor.
              byteread += tmp_read;
              BytesLeftToRead = BytesLeftToRead - tmp_read;//PresContext.Size();
              m_proposedPresentationContexts.push_back(PresContext);
            }
          break;
          case 0x10:
            {
              ApplicationContext app_context(APPLICATION_CONTEXT);
              tmp_read = app_context.readDynamic(socket);
              byteread += tmp_read;
              BytesLeftToRead = BytesLeftToRead - tmp_read;//app_context.Size();
              if (app_context.m_UID != APPLICATION_CONTEXT)
              {
                throw exception("Unsupported Application Context");//this should be throw AssociationException(...)
              }
            }
          break;
          default:
            throw BadItemType(tmpBYTE, 0);
        }
      }

      if(BytesLeftToRead)
      {
        throw dicom::exception("BytesLeftToRead != 0 at end of aarq read." );
      }

      return byteread;
    }

    UINT32 AAssociateRQ::size()
    {
      UINT32 length;
      length = sizeof(UINT16) + sizeof(UINT16) + 16 + 16 + 32;
      length += m_appContext.size();
      size_t Index = 0;

      while(Index < m_proposedPresentationContexts.size())
      {
        length += m_proposedPresentationContexts[Index].size();
        ++Index;
      }

      length += m_userInfo.size();
      return (length + sizeof(BYTE) + sizeof(BYTE) + sizeof(UINT32));
    }
  }//namespace primitive
}//namespace dicom
