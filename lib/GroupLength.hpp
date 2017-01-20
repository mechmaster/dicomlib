/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/


#ifndef GROUP_LENGTH_HPP_INCLUDE_GUARD_57903648AOUQK879
#define GROUP_LENGTH_HPP_INCLUDE_GUARD_57903648AOUQK879

#include "DataSet.hpp"
#include "TransferSyntax.hpp"

namespace dicom
{
	UINT32 GroupLength(const dicom::DataSet& data,dicom::TS ts);

}//namespace dicomlib


#endif//GROUP_LENGTH_HPP_INCLUDE_GUARD_57903648AOUQK879
