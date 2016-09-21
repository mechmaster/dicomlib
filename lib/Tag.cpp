/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "Tag.hpp"
#include <sstream>
#include "DataDictionary.hpp"
namespace dicom
{
	 
    const char* TagNotFound::what() const throw(){
    	std::ostringstream os;
    	os << "Tag Not Found: " << GetName(tag_);
    	return os.str().c_str();
    }
}
