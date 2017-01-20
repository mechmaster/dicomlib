#ifndef UTILITY_HPP_INCLUDE_GUARD_45475351328
#define UTILITY_HPP_INCLUDE_GUARD_45475351328
#include <string>
#include <limits>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include "Types.hpp"

//these should be in dicom namespace

//!Removes leading and trailing whitespace from a string.
void StripTrailingWhitespace(std::string& str);

void StripTrailingNull(std::string& str);

bool IsDigitString(std::string& str);

//The following are GENERATORS//

template <typename T>
struct Series
{
	T CurrentValue_;
	Series():CurrentValue_(0){}
	T operator()()
	{
		return CurrentValue_++;
	}
};

template <typename T>
struct OddSeries
{
	T CurrentValue_;
	OddSeries():CurrentValue_(1){}
	T operator()()
	{
		T ret=CurrentValue_;
		CurrentValue_+=2;
		return ret;
	}
};

//UINT8		uniq8();
//UINT8		uniq8odd();
UINT16		uniq16odd();

#endif //UTILITY_HPP_INCLUDE_GUARD_45475351328
