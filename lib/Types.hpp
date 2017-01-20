#ifndef CPP_TYPES_HPP_INCLUDE_GUARD_74749238
#define CPP_TYPES_HPP_INCLUDE_GUARD_74749238
#include <boost/static_assert.hpp>
/* C++ Types */
/*

	Use this file to specify fixed-length types.
	(Note that c++ defines relationships between type sizes,
	e.g. sizeof(short) <= sizeof(long), but does NOT define
	a byte count for them.  DICOM, however, specifies a fixed
	byte length for it's fundamental types, so we can avoid
	ambiguities by using guaranteed length types like UINT32
	rather than unsigned int.
*/


//#ifdef _WIN32
//	#include "BaseTsd.h"
//#else
	typedef unsigned	char	UINT8;
	typedef unsigned	short	UINT16;
	typedef unsigned	int		UINT32;
	typedef unsigned	int		UINT32;
//#endif

typedef UINT8 BYTE;


BOOST_STATIC_ASSERT(sizeof(UINT8)==1);
BOOST_STATIC_ASSERT(sizeof(UINT16)==2);
BOOST_STATIC_ASSERT(sizeof(UINT32)==4);


#endif //CPP_TYPES_HPP_INCLUDE_GUARD_74749238
