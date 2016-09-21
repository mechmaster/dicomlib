/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "UID.hpp"
#include <cctype>
#include <sstream>
#include <time.h>
#include <algorithm>
//#include "iso646.h"
#include "Utility.hpp"
namespace dicom
{
	namespace
	{

		//!UIDs can only contain [0-9],'.' and '*' (when wildcards are being used in a C-FIND query)
		void ThrowIfInvalid(const char c)
		{
			// !since Mayo Clinic DICOM images contain '-'in UID, 
			// !it's violation of the standard, however, 
			// !Gord asked me to break the law, Apr 24, 2006
			//if(c!='.' && !std::isdigit(c) && c!='*' )
			if(c!='.' && !std::isdigit(c) && c!='*' && c!='-' )
			{
				std::ostringstream s;
				s << c << " is an invalid UID character";
				throw InvalidUID(s.str());
			}
		}
	}
	/*!
		a UID can only be created from a std::string object.
		The construct verifies that the string conforms to
		the standard description of unique identifiers.

	*/
	UID::UID(const std::string& s) : data_(s)
	{
		if(data_.size()>64)
		{
			std::ostringstream error;
			error << "UID : " << s << " is too long";
			throw InvalidUID(error.str());
		}
		// Following line was previously commented out.
		StripTrailingWhitespace(data_);
		StripTrailingNull(data_);
		std::for_each(data_.begin(),data_.end(),ThrowIfInvalid);

	}
	std::string UID::str()const
	{
		return data_;
	}

	InvalidUID::InvalidUID(std::string what):dicom::exception(what)
	{

	}

	/*!
		Generates a (hopefully!) unique UID.
		This may not be thread safe.
	*/

	UID makeUID(const std::string &Prefix)
	{
		std::ostringstream stream;
		stream	<< Prefix
				<< "."		<<	time(0)
				<< "."		<<	clock()
				<< "."		<<	rand();
		std::string s=stream.str();
		if(s.size()>64)
			throw dicom::exception("Generated UID is larger than 64 characters.");
		return UID(s);

	}


}//namespace dicom
