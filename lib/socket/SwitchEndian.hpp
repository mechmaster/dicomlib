#ifndef SWITCH_ENDIAN_HPP_INCLUDE_GUARD_2304875234560
#define SWITCH_ENDIAN_HPP_INCLUDE_GUARD_2304875234560
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <algorithm>

//!Reverses the bytes in a variable.
/*!
	It is possible that we could heavily optimise endian switching using
	the swab system call.

	(What namespace should this be in?)
*/


#ifdef _WIN32
	typedef char SwabType;
#else
	typedef void SwabType;
#endif

inline
void SwitchVectorEndian(std::vector<unsigned short>& data)
{
	BOOST_STATIC_ASSERT(sizeof(unsigned short)==2);

	SwabType* p_data=reinterpret_cast <SwabType*>(&data[0]);

	swab(p_data,
		p_data,
		static_cast<int>(data.size()*2));
}



/*
	May cause obscure linker errors under vc7.0, see discussion at
	http://groups.google.ca/groups?hl=en&lr=&ie=UTF-8&oe=UTF-8&threadm=4ac23acc.0301190831.34470124%40posting.google.com&rnum=20&prev=/groups%3Fq%3Dlnk1120%2Btemplate%2Bfunction%26hl%3Den%26lr%3D%26ie%3DUTF-8%26oe%3DUTF-8%26start%3D10%26sa%3DN
*/

template <typename T>
inline T SwitchEndian(T value)
{
	BOOST_STATIC_ASSERT(::boost::is_arithmetic<T>::value);//either a float or an integral...
	BOOST_STATIC_ASSERT(!::boost::is_const<T>::value);
	unsigned char* b=(unsigned char*)(&value);
	std::reverse(b,b+sizeof(T));//alternatively could use swab()
	return value;
}




#endif //SWITCH_ENDIAN_HPP_INCLUDE_GUARD_2304875234560

