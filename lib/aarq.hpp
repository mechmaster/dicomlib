/************************************************************************
* DICOMLIB
* Copyright 2003 Sunnybrook and Women's College Health Science Center
* Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
* See LICENSE.txt for copyright and licensing info.
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
#include "Buffer.hpp"

/*
* PDU Service Classes:
* A-ASSOCIATE-RQ Class.
*/

namespace dicom
{
  //!Thrown if we get fed a PDU we're not expecting.
  struct BadItemType : public dicom::exception
  {
    //!What was provided.
    std::uint8_t m_item;
    //!What should have been provided, or 0 if unknown.
    std::uint8_t m_expected;

    BadItemType(std::uint8_t Item, std::uint8_t Expected) :
      dicom::exception("Bad Item Type"),
      m_item(Item),
      m_expected(Expected)
    {
    }
  };

  //!Throws BadItemType if Given is not equal to Expected
  void EnforceItemType(std::uint8_t Given, std::uint8_t Expected);

  namespace primitive
  {
    /*!
    Defined in Part 8, table 9-12
    */
    struct ApplicationContext
    {
      static const std::uint8_t m_itemType = 0x10;
      static const std::uint8_t m_reserved = 0x00;

      UID m_UID;

      ApplicationContext(const UID&);

      std::uint32_t read(Buffer& temp);
      void write(Buffer& temp);
      std::uint32_t size();
    };

    /*!
    Defined in Part 8, table 9-14
    */
    struct AbstractSyntax
    {
      static const std::uint8_t m_itemType = 0x30;
      static const std::uint8_t m_reserved = 0x00;

      UID m_UID;

      AbstractSyntax(const UID &);

      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint32_t size();
    };

    /*
    * What is the functional overlap between this class and the one named "TS"??
    * This is the 'message' that gets exchanged along the wire.  'TS' is our internal
    * representation of the Transfer Syntax concept.
    */
    /*!
    Defined in Part 8, table 9-15
    */
    struct TransferSyntax
    {
      static const std::uint8_t m_itemType = 0x40;
      static const std::uint8_t m_reserved = 0x00;

      UID m_UID;

      TransferSyntax(const UID&);

      void set(const UID&);
      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint32_t size();
    };

    //!Identifies an implementation by unique identifier
    /*!
    Defined in Part 7, table D.3-2
    */
    struct ImplementationClass
    {
      static const std::uint8_t m_itemType = 0x52;
      static const std::uint8_t m_reserved = 0x00;

      UID m_UID;

      ImplementationClass(const UID&);
      std::uint32_t read(Buffer& temp);
      void write(Buffer& temp);
      std::uint32_t size();
    };

    //!Identifies a particuler imlementation by name
    /*!
    Documented in Part 7, Annex D.3.3.2.3, especially
    table D.3-3
    */
    struct ImplementationVersion
    {
      static const std::uint8_t m_itemType = 0x55;
      static const std::uint8_t m_reserved = 0x00;

      std::string m_name;//May be 1 to 16 characters long.

      std::uint32_t read(Buffer& temp);
      void write(Buffer& temp);
      std::uint32_t size();
    };

    /*!
    Part 7, Table D.3-9
    */
    struct SCPSCURoleSelect
    {
      static const std::uint8_t m_itemType = 0x54;
      static const std::uint8_t m_reserved = 0x00;

      std::uint16_t m_itemLength;//length to end of object - probably shouldn't be a member
      UID m_UID;

      std::uint8_t m_SCURole;
      std::uint8_t m_SCPRole;

      SCPSCURoleSelect();//is this a good idea?

      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint32_t size();
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

      static const std::uint8_t m_itemType = 0x20;
      static const std::uint8_t m_reserved1 = 0x00;
      static const std::uint8_t m_reserved2 = 0x00;
      static const std::uint8_t m_reserved3 = 0x00;
      static const std::uint8_t m_reserved4 = 0x00;

      std::uint16_t m_length;
      std::vector<TransferSyntax> m_transferSyntaxes;

      std::uint8_t m_ID;
      AbstractSyntax m_AbsSyntax;

      PresentationContext();
      PresentationContext(const AbstractSyntax&, const std::vector<TransferSyntax>&, std::uint8_t id);
      PresentationContext(const AbstractSyntax&, std::uint8_t id);

      void addTransferSyntax(TransferSyntax&);
      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint32_t size();
    };

    /*!
    Defined in Part 8, tables D.1-1 and D.1-2
    */
    struct MaximumSubLength
    {
      static const std::uint8_t m_itemType = 0x51;
      static const std::uint8_t m_reserved1 = 0x00;
      static const std::uint16_t m_length = 0x04;
      std::uint32_t m_maximumLength;

      MaximumSubLength();
      MaximumSubLength(std::uint32_t);

      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint32_t size();
    };

    /*!
    Defined in Part 8/table 9-16
    */
    struct UserInformation
    {
      static const std::uint8_t m_itemType = 0x50;
      static const std::uint8_t m_reserved = 0x00;

      std::uint32_t m_userInfoBaggage;
      MaximumSubLength m_maxSubLength;
      ImplementationClass m_impClass;
      ImplementationVersion m_impVersion;//this is an optional field. 

      //this is an optional field.  How do we indicate that?
      SCPSCURoleSelect m_SCPSCURole;

      UserInformation();

      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint32_t size();
    };

    //!A request to open a dicom association
    /*!
    Defined in Part 8, table 9-11
    */
    struct AAssociateRQ
    {
      static const std::uint8_t m_itemType = 0x01;
      static const std::uint8_t m_reserved1 = 0x00;
      static const std::uint16_t m_protocolVersion = 0x01;
      static const std::uint16_t m_reserved2 = 0x00;

      std::string m_calledAppTitle;
      std::string m_callingAppTitle;
      std::array<std::uint8_t, 32> m_reserved3;

      ApplicationContext m_appContext;

      //! I think this should be called "ProposedPresentationContexts"
      std::vector<PresentationContext> m_proposedPresentationContexts; //PresContexts;
      UserInformation m_userInfo;

      AAssociateRQ();
      AAssociateRQ(const std::string& CallingAp, const std::string& CalledAp);

      void setUserInformation(UserInformation&);
      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint32_t size();
    };
  }//namespace primitive
}//namespace dicom
