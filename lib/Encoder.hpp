#ifndef ENCODER_HPP_INCLUDE_GUARD_5729564748
#define ENCODER_HPP_INCLUDE_GUARD_5729564748
#include <exception>
#include "DataSet.hpp"
#include "socket/Socket.hpp"
#include "TransferSyntax.hpp"
#include "Buffer.hpp"
/*
	As the following class basically only performs one job,
	maybe it should expose itself as a simple function call?
	See comments in Decoder.hpp
*/
namespace dicom
{
	struct EncoderError : public std::exception{};



	UINT32 WriteToBuffer(const DataSet& data, Buffer& buffer, TS transfer_syntax);

}//namespace dicom


#endif//ENCODER_HPP_INCLUDE_GUARD_5729564748
