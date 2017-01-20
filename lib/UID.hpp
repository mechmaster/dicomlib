#ifndef UID_HPP_INCLUDE_GUARD_839450398450
#define UID_HPP_INCLUDE_GUARD_839450398450

#include "Types.hpp"
#include "Utility.hpp"
#include <string>
#include "Exceptions.hpp"

namespace dicom
{
	//!Thrown if construction of UID fails.
	struct InvalidUID:public dicom::exception
	{
		InvalidUID(std::string what="Invalid UID");

		virtual ~InvalidUID() throw(){}

	};

	//!	Unique Identifier class.
	/*!
		Specified in Part 8, Annex F.1 (page 49)

		A UID is basically a string of up to 64 characters, which can be the numbers 0-9 or a period ('.')
	*/

	struct UID
	{
		//!Verifying constructor
		UID(const std::string& s="");

		//!Access underlying string representation.
		std::string str()const;

		//!So we can sort on UID
		bool operator < (const UID& comp)const
		{
			return (data_  < comp.str());
		}
		bool operator == (const UID& comp) const
		{
			return (data_ == comp.str());
		}
		bool operator !=(const UID& comp) const
		{
			return data_!=comp.str();
		}
	private:
		 std::string data_;
	};


	//!Create a random, hopefully-unique UID with the supplied prefix.
	UID makeUID(const std::string& Prefix);
}//namespace dicom
#endif //UID_HPP_INCLUDE_GUARD_839450398450
