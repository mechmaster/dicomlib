/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#pragma once

/*
	I'd like to pull most of these classes out into individual files,
	so I can actually find the damn things when I want them.

	Either that or put them all in DicomMessages.hpp?
*/

/*
	This file should be renamed something
	more obvious, like AssociationRequest.

	Some of the other stuff in this file may not
	belong here.
*/

/*
	I don't seem to have been consistent with my naming conventions
	-sometimes I use a trailing underscore to denote membership, but
	not always.
*/

/*
	I'm not happy that message primitive classes have default constructors.
	The procedure for reading messages is often:
	Create object with default constructor, then call member function ReadDynamic
	with a socket object as an argument.
	Why not just have constructors that take socket objects as arguments?

*/

#include <array>
#include <string>
#include <vector>

#include "UID.hpp"
#include "Types.hpp"

/*
* PDU Service Classes:
*	A-ASSOCIATE-RQ Class.
*/

namespace dicom
{
  //!Thrown if we get fed a PDU we're not expecting.
  struct BadItemType : public dicom::exception
  {
    //!What was provided.
    BYTE m_item;
    //!What should have been provided, or 0 if unknown.
    BYTE m_expected;

    BadItemType(BYTE Item, BYTE Expected) :
      dicom::exception("Bad Item Type"),
      m_item(Item),
      m_expected(Expected)
    {
    }
  };

  //!Throws BadItemType if Given is not equal to Expected
  void EnforceItemType(BYTE Given, BYTE Expected);

  namespace primitive
  {
    /*!
    Defined in Part 8, table 9-12
    */
    struct ApplicationContext
    {
      static const BYTE m_itemType = 0x10;
      static const BYTE m_reserved = 0x00;

      UID m_UID;

      ApplicationContext(const UID&);

      UINT32 readDynamic(Network::Socket& socket);
      void write(Network::Socket& socket) const;
      UINT32 size() const;
    };

    /*!
    Defined in Part 8, table 9-14
    */
    struct AbstractSyntax
    {
      static const BYTE m_itemType = 0x30;
      static const BYTE m_reserved = 0x00;

      UID m_UID;

      AbstractSyntax(const UID &);

      void set(const UID&);
      void write(Network::Socket&);
      UINT32 read(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT32 size();
    };

    /*
    *	What is the functional overlap between this class and the one named "TS"??
    *	This is the 'message' that gets exchanged along the wire.  'TS' is our internal
    *	representation of the Transfer Syntax concept.
    */
    /*!
    Defined in Part 8, table 9-15
    */
    struct TransferSyntax
    {
      static const BYTE m_itemType = 0x40;
      static const BYTE m_reserved = 0x00;

      UID m_UID;

      TransferSyntax(const UID&);

      void set(const UID&);
      void write(Network::Socket&);
      UINT32 read(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT32 size();
    };

    //!Identifies an implementation by unique identifier
    /*!
    Defined in Part 7, table D.3-2
    */
    struct ImplementationClass
    {
      static const BYTE m_itemType = 0x52;
      static const BYTE m_reserved = 0x00;

      UID m_UID;

      ImplementationClass(const UID&);
      UINT32 readDynamic(Network::Socket& socket);
      void write(Network::Socket& socket);
      UINT32 size();
    };

    //!Identifies a particuler imlementation by name
    /*!
    Documented in Part 7, Annex D.3.3.2.3, especially
    table D.3-3
    */
    struct ImplementationVersion
    {
      static const BYTE m_itemType = 0x55;
      static const BYTE m_reserved = 0x00;

      std::string m_name;//May be 1 to 16 characters long.

      UINT32 readDynamic(Network::Socket& socket);
      void write(Network::Socket& socket);
      UINT32 size();
    };

    /*!
    Part 7, Table D.3-9
    */
    struct SCPSCURoleSelect
    {
      static const BYTE m_itemType = 0x54;
      static const BYTE m_reserved = 0x00;

      UINT16 m_itemLength;//length to end of object - probably shouldn't be a member
      UID m_UID;

      BYTE m_SCURole;
      BYTE m_SCPRole;

      SCPSCURoleSelect();//is this a good idea?

      void write(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT32 size();
    };

    /*!
    defined in Part 8, table 9-13
    */
    struct PresentationContext
    {
      /*
      As far as I can tell, the C++ standard requires that if we ever take the _address_
      of a static const member (e.g. pass it by reference), then we actually need to
      instantiate it somewhere.  Practically, only gcc seems to enforce this - MSVC seems
      to ignore the requirement.
      */

      static const BYTE m_itemType = 0x20;
      static const BYTE m_reserved1 = 0x00;
      static const BYTE m_reserved2 = 0x00;
      static const BYTE m_reserved3 = 0x00;
      static const BYTE m_reserved4 = 0x00;

      UINT16 m_length;
      std::vector<TransferSyntax> m_transferSyntaxes;

      BYTE m_ID;
      AbstractSyntax m_AbsSyntax;

      PresentationContext();
      PresentationContext(const AbstractSyntax&, const std::vector<TransferSyntax>&, BYTE id);
      PresentationContext(const AbstractSyntax&, BYTE id);

      void addTransferSyntax(TransferSyntax&);
      void write(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT32 size();
    };

    /*!
    Defined in Part 8, tables D.1-1 and D.1-2
    */
    struct MaximumSubLength
    {
      static const BYTE m_itemType = 0x51;
      static const BYTE m_reserved1 = 0x00;
      static const UINT16 m_length = 0x04;
      UINT32 m_maximumLength;

      MaximumSubLength();
      MaximumSubLength(UINT32);

      void set(UINT32);
      UINT32 get();
      void write(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT32 size();
    };

    /*!
    Defined in Part 8/table 9-16
    */
    struct UserInformation
    {
      static const BYTE m_itemType = 0x50;
      static const BYTE m_reserved = 0x00;

      UINT32 m_userInfoBaggage;
      MaximumSubLength m_maxSubLength;
      ImplementationClass m_impClass;
      ImplementationVersion m_impVersion;//this is an optional field. 

      //this is an optional field.  How do we indicate that?
      SCPSCURoleSelect m_SCPSCURole;

      UserInformation();

      void setMax(MaximumSubLength&);

      void write(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT16 size();
    };

    //!A request to open a dicom association
    /*!
    Defined in Part 8, table 9-11
    */
    struct AAssociateRQ
    {
      static const BYTE m_itemType = 0x01;
      static const BYTE m_reserved1 = 0x00;
      static const UINT16 m_protocolVersion = 0x01;
      static const UINT16 m_reserved2 = 0x00;

      std::string m_calledAppTitle;
      std::string m_callingAppTitle;
      std::array<BYTE, 32> m_reserved3;

      const ApplicationContext m_appContext;

      //! I think this should be called "ProposedPresentationContexts"
      std::vector<PresentationContext> m_proposedPresentationContexts;//PresContexts;
      UserInformation m_userInfo;

      AAssociateRQ();
      AAssociateRQ(const std::string& CallingAp, const std::string& CalledAp);

      void setUserInformation(UserInformation&);
      void write(Network::Socket&);
      UINT32 read(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT32 size();
    };
  }//namespace primitive
}//namespace dicom
