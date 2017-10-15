/************************************************************************
* DICOMLIB
* Copyright 2003 Sunnybrook and Women's College Health Science Center
* Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
* See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#pragma once

#include "DataSet.hpp"
#include "TransferSyntax.hpp"

namespace dicom
{
  std::uint32_t GroupLength(const dicom::DataSet& data, dicom::TS ts);
}//namespace dicomlib
