/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/


#include "GroupLength.hpp"
#include "Encoder.hpp"
namespace dicom
{
	//!Figure out the length in bytes of a dataset by writing it to a temporary buffer.
	/*!
		This is probably an inefficient way of doing this, but generally
		speaking we'll only use it with small datasets like DIMSE commands
		and file meta-information, of the order of a couple
		of hundred bytes at most, so I don't think this will have
		much impact on performance.

		Note that the group length is dependent on which transfer syntax we use, as
		explicit vr writes more info than implicit vr.
	*/

	UINT32 GroupLength(const DataSet& data,TS ts)
	{
		int ByteOrder=ts.isBigEndian()?
			__BIG_ENDIAN:__LITTLE_ENDIAN;

		Buffer buffer(ByteOrder);
		WriteToBuffer(data,buffer,ts);
		return (buffer.size());
	}

}//namespace dicomlib
