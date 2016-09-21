#ifndef BASE_HPP_INCLUDE_GUARD_537304573409
#define BASE_HPP_INCLUDE_GUARD_537304573409

/*
	Operating-system specific definitions go here.
	Currently we support windows, linux (x86) and solaris.
	Mostly this file is used for detecting host endian-ness.
	If there's a cleaner way of doing this, please let me know

	Basically we're trying to make every system have the same
	definitions that linux presents in <endian.h>, that is,
	__LITTLE_ENDIAN, __BIG_ENDIAN, and __BYTE_ORDER
*/

#if defined( __unix__)

	#if(defined(__sun__))
		#if(defined(__sparc))
			#include <sys/byteorder.h>
			#define __LITTLE_ENDIAN 1234
			#define __BIG_ENDIAN 4321
			#ifdef _BIG_ENDIAN
				#define __BYTE_ORDER __BIG_ENDIAN
			#elif
				#error "unrecognized sparc architecture."
			#endif
		#elif
			#error "Unsupported architecture"
		#endif
	#elif(defined(__linux__))
		#include "endian.h"
		#if(defined(__i386) || defined(__x86_64))    
			//We support this.
		#else
			//this can be fixed by checking for the appropriate architecture
			//signifier macro - I just don't know off hand what these are.
			#error "Unsupported architecture."
		#endif
	#else
		#error "Unknown Unix system."
	#endif
#elif defined(_WIN32)
	#pragma warning(disable:4290)  // Disable warning about throw specifications

//	#include "iso646.h"//for 'bitand' etc.
//	#if(defined(_M_IX86))
		//Windows doesn't have "Endian.h", so we declare these manually.
		#define __LITTLE_ENDIAN 1234
		#define __BIG_ENDIAN 4321
		#define __BYTE_ORDER __LITTLE_ENDIAN

//	#else
//		#error "Unsupported architecture."
//	#endif
#else
	#error "Unknown operating system"
#endif



#endif //BASE_HPP_INCLUDE_GUARD_537304573409
