/************************************************************************
* DICOMLIB
* Copyright 2003 Sunnybrook and Women's College Health Science Center
* Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
* See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <algorithm>
#include <sstream>

#include "UID.hpp"
#include "UIDs.hpp"
#include "Exceptions.hpp"
#include "TransferSyntax.hpp"

namespace dicom
{
  TS::TS(const UID& uid) :
    m_uid(uid)
  {
    //make sure uid represents a known transfer syntax.
    dicom::Enforce(
    IMPL_VR_LE_TRANSFER_SYNTAX == uid ||
    EXPL_VR_LE_TRANSFER_SYNTAX == uid ||
    DEFLATED_EXPL_VR_LE_TRANSFER_SYNTAX == uid ||
    EXPL_VR_BE_TRANSFER_SYNTAX == uid ||
    JPEG_BASELINE_TRANSFER_SYNTAX == uid ||
    JPEG_LOSSLESS_NON_HIERARCHICAL == uid ||
    JPEG2000_LOSSLESS_ONLY == uid ||
    JPEG2000 == uid, "Syntax not recognised: " + uid.str());
  }

  UID TS::getUID() const
  {
    return m_uid;
  }

  /*!
  Part 5, Annex 4 (a) says we use Explicit VR, Little endian for jpeg encoded syntaxs.
  */

  bool TS::isExplicitVR() const
  {
    return (IMPL_VR_LE_TRANSFER_SYNTAX != m_uid);
  }

  bool TS::isBigEndian() const
  {
    return (EXPL_VR_BE_TRANSFER_SYNTAX == m_uid);
  }

  bool TS::isDeflated() const
  {
    return (DEFLATED_EXPL_VR_LE_TRANSFER_SYNTAX == m_uid);
  }

  /*!
  Does this transfer syntax indicate that pixel data is stored
  in encapsulated encoded form, as described in Part 5 annex 4?
  */

  bool TS::isEncapsulated() const
  {
    return (
    JPEG_BASELINE_TRANSFER_SYNTAX == m_uid ||
    JPEG_LOSSLESS_NON_HIERARCHICAL == m_uid ||
    JPEG2000_LOSSLESS_ONLY == m_uid ||
    JPEG2000 == m_uid
    );
  }
}//namespace dicom
