#pragma once

#include <string>

#include "Tag.hpp"
#include "VR.hpp"
#include "Exceptions.hpp"

namespace dicom
{
  //!Thrown if we don't recognize a given tag.
  struct UnknownTag : public exception
  {
    Tag m_tag;

    UnknownTag(Tag tag) : exception("Unknown Tag."), m_tag(tag)
    {
    }
  };

  //!Get VR associated with given tag.
  VR GetVR(Tag tag);

  //!Get human-readable name for given tag.
  std::string GetName(Tag tag);
  std::string GetTagString(Tag tag);

  //!Insert custom entries into the data dictionary at runtime
  void AddDictionaryEntry(Tag tag, VR vr, std::string name);
}//namespace dicom
