#ifndef DECODER_HPP_INCLUDE_GUARD_5823561955
#define DECODER_HPP_INCLUDE_GUARD_5823561955

#include "DataSet.hpp"
#include "socket/Socket.hpp"
#include "TransferSyntax.hpp"
#include "Exceptions.hpp"
#include "Buffer.hpp"
/*
	TODO
	
	This file presents an unnecessarily complicated interface to the 
	end user.  Pleas simplify.
	
		hint - The user probably doesn't need to know about Decoder
		objects - the interface could be reduced to:
	Decode(Buffer&,DataSet&,TS);
*/
namespace dicom
{
	struct DecoderError : public dicom::exception
	{
		DecoderError(std::string description):dicom::exception(description){}
		virtual ~DecoderError()throw(){}
	};
	
	//!This function seems only to be used by FileMetaInformation
	void ReadElementFromBuffer(Buffer& buffer, DataSet& data,TS transfer_syntax);
	
	void ReadFromBuffer(Buffer& buffer, DataSet& data, TS transfer_syntax);

}//namespace dicom
#endif //DECODER_HPP_INCLUDE_GUARD_5823561955
