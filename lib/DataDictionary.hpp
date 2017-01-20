#ifndef DATADICTIONARY_HPP_INCLUDE_GUARD_94829374
#define DATADICTIONARY_HPP_INCLUDE_GUARD_94829374

#include "Tag.hpp"
#include "VR.hpp"
#include "Exceptions.hpp"
#include <map>
namespace dicom
{
	//!Thrown if we don't recognize a given tag.
	struct UnknownTag : public exception
	{
		Tag tag_;
		UnknownTag(Tag tag) : exception("Unknown Tag."),tag_(tag){}
		//virtual ~UnknownTag()throw(){}
	};

	//!Get VR associated with given tag.
	VR GetVR(Tag tag);

	//!Get human-readable name for given tag.
	std::string GetName(Tag tag) ;

	std::string GetTagString(Tag tag) ;

	//!Insert custom entries into the data dictionary at runtime
	void AddDictionaryEntry(Tag tag, VR vr, std::string name);
}//namespace dicom
#endif // DATADICTIONARY_HPP_INCLUDE_GUARD_94829374
