/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/
#pragma once

#include <array>
#include <vector>

#include "aarq.hpp"
#include "Types.hpp"
#include "Buffer.hpp"

namespace dicom
{
  //!message primitives, as described in Part 8, section 9
  namespace primitive
  {
    /*!
    Defined in Part 8, table 9-18
    */
    class PresentationContextAccept
    {
      static const std::uint8_t m_itemType = 0x21;
      static const std::uint8_t m_reserved1 = 0x00;
      static const std::uint8_t m_reserved2 = 0x00;
      static const std::uint8_t m_reserved4 = 0x00;

    public:
      
      std::uint8_t m_result;
      std::uint8_t m_presentationContextID;

      TransferSyntax m_trnSyntax;

      PresentationContextAccept();

      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint16_t size();
    };

    //!Should document where this is defined
    class AAssociateAC
    {
      static const std::uint8_t m_itemType = 0x02;
      static const std::uint8_t m_reserved1 = 0x00;
      static const std::uint16_t m_protocolVersion = 0x01;
      static const std::uint16_t m_reserved2 = 0x00;

    public:

      std::string m_calledAppTitle;
      std::string m_callingAppTitle;
      std::array<std::uint8_t, 32> m_reserved3;
      
      ApplicationContext m_appContext;
      std::vector<PresentationContextAccept> m_presContextAccepts;
      UserInformation m_userInfo;

      AAssociateAC();
      AAssociateAC(const std::string& callingApp, const std::string& calledApp);

      void setUserInformation(UserInformation&);
      void write(Buffer& temp);
      std::uint32_t read(Buffer& temp);
      std::uint32_t size();
    };
  }//namespace primitive
}//namespace dicom
