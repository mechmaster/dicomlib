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

namespace dicom
{
  //!message primitives, as described in Part 8, section 9
  namespace primitive
  {
    /*!
    Defined in Part 8, table 9-18
    */
    struct PresentationContextAccept
    {
      static const BYTE m_itemType = 0x21;
      static const BYTE m_reserved1 = 0x00;
      static const BYTE m_reserved2 = 0x00;
      static const BYTE m_reserved4 = 0x00;

      BYTE m_result;
      BYTE m_presentationContextID;

      TransferSyntax m_trnSyntax;

      PresentationContextAccept();

      void write(Network::Socket&);
      UINT32 read(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT16 size();
    };

    //!Should document where this is defined
    class AAssociateAC
    {
      static const BYTE m_itemType = 0x02;
      static const BYTE m_reserved1 = 0x00;
      static const UINT16 m_protocolVersion = 0x01;
      static const UINT16 m_reserved2 = 0x00;

    public:

      std::string m_calledAppTitle;
      std::string m_callingAppTitle;
      std::array<BYTE, 32> m_reserved3;
      
      ApplicationContext m_appContext;
      std::vector<PresentationContextAccept> m_presContextAccepts;
      UserInformation m_userInfo;

      AAssociateAC();
      AAssociateAC(const std::string& callingApp, const std::string& calledApp);

      void setUserInformation(UserInformation&);
      void write(Network::Socket&);
      UINT32 read(Network::Socket&);
      UINT32 readDynamic(Network::Socket&);
      UINT32 size();
    };
  }//namespace primitive
}//namespace dicom
