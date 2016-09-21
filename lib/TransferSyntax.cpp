/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <algorithm>
#include "TransferSyntax.hpp"
#include "UID.hpp"
#include "UIDs.hpp"
#include "Exceptions.hpp"
#include <sstream>
namespace dicom
{


	TS::TS(const UID& uid):uid_(uid)
	{
		//make sure uid represents a known transfer syntax.
		dicom::Enforce(
			IMPL_VR_LE_TRANSFER_SYNTAX == uid			||
			EXPL_VR_LE_TRANSFER_SYNTAX == uid			||
			DEFLATED_EXPL_VR_LE_TRANSFER_SYNTAX == uid	||
			EXPL_VR_BE_TRANSFER_SYNTAX == uid			||
			JPEG_BASELINE_TRANSFER_SYNTAX == uid		||
			JPEG_LOSSLESS_NON_HIERARCHICAL == uid		||
			JPEG2000_LOSSLESS_ONLY == uid               ||
            JPEG2000 == uid
			,"Syntax not recognised: " + uid.str());

	}

	UID TS::getUID() const
	{
		return uid_;
	}


	/*!
		Part 5, Annex 4 (a) says we use Explicit VR, Little endian for jpeg encoded syntaxs.
	*/

	bool TS::isExplicitVR() const
	{
		return (IMPL_VR_LE_TRANSFER_SYNTAX!=uid_);
	}

	bool TS::isBigEndian() const
	{
		return (EXPL_VR_BE_TRANSFER_SYNTAX==uid_);
	}

	bool TS::isDeflated() const
	{
		return (DEFLATED_EXPL_VR_LE_TRANSFER_SYNTAX==uid_);
	}

	/*!
		Does this transfer syntax indicate that pixel data is stored
		in encapsulated encoded form, as described in Part 5 annex 4?
	*/
	bool TS::isEncapsulated() const
	{
		return (
			JPEG_BASELINE_TRANSFER_SYNTAX==uid_||
			JPEG_LOSSLESS_NON_HIERARCHICAL==uid_ ||
			JPEG2000_LOSSLESS_ONLY==uid_ ||
            JPEG2000 ==uid_
			);
	}
}//namespace dicom
