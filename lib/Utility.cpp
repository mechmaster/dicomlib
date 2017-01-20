/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include	"Utility.hpp"
#include <algorithm>


void StripTrailingWhitespace(std::string& str)
{
	std::string::size_type last=str.find_last_not_of(' ');
	if(last==std::string::npos)
		last=0;
	else
		last++;
	
	str.resize(last);


	/*
		Technically speaking, we shouldn't have to do this, but we have
		encountered some GE images that have NULLS at the end of strings
		rather than whitespace.
	
	*/
	StripTrailingNull(str);
	
}


void StripTrailingNull(std::string& str)
{
	std::string::size_type last=str.find_last_not_of('\0');
	if(last==std::string::npos)
		last=0;
	else
		last++;
	str.resize(last);
}

bool IsDigitString(std::string& str)
{
	for(std::string::const_iterator I=str.begin(); I!=str.end(); I++)
		if(isdigit(*I)==0)
			return false;

	return true;
}


unsigned	long	uniqid = 1;


/*
	Occasionaly the standard calls for a number unique to a given
	association (see e.g. Part 8, Section 7.1.1.13)

	The implementation here is insufficiently robust - the above variable
	'uniqid' should be a member of the object responsible for negotiating
	the association.
*/



//UINT8	uniq8()
//{
//	return((uniqid++)%0xff);
//}
//
//UINT8	uniq8odd()
//{
//	if(uniqid & 0x01)
//		return((uniqid++)%0xff);
//	++uniqid;
//	return((uniqid++)%0xff);
//}

UINT16	uniq16odd()
{
	if(uniqid & 0x01)
		return((uniqid++)%0xffff);
	++uniqid;
	return((uniqid++)%0xffff);
}

