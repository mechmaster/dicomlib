#pragma once

#include <map>

#include "VR.hpp"
#include "Value.hpp"
#include "Tag.hpp"

namespace dicom
{
//!Set of DICOM data elements
/*!

An element in a DataSet is a tag-value pair.

Multiple elements may have the same tag.  This is called
'value multiplicity' in DICOM terminology (see PS3.5/section 6.4).

We implement this by using a std::multimap, and for convenience
provide a couple of accessor functions.  Note that you can still
always use any of the functions that std::multimap provides, such
as equal_range(), count() and find()

*/

  class DataSet : public std::multimap<Tag, Value>
  {

  public:

  //!access an element
  /*!
  This isn't ideal, as it will only return the FIRST
  element that matches Tag, even if there are more than one.  (Value
  multiplicity).  Use equal_range() or count() to find the multiplicity
  of a given tag.
  */

    const Value& operator()(const Tag tag) const
    {
      const_iterator element = find(tag);

      if(element == end())
      {
        throw TagNotFound(tag);
      }

      return element->second;
    }

  //!Insert an element
  /*! I'd rather that the function signature was:
  template <VR vr> size_t Put(Tag tag, const typename TypeFromVR<vr>::Type& data) but VisualStudio doesn't like it.
  */

    template <VR vr, typename T>
    void put(Tag tag, const T& data)
    {
      StaticVRCheck<T, vr>();
      Value v(vr, data);
      insert(value_type(tag, v));
    }

  /** Is it safe to use???
  */

    //!Insert an element with zero data length
    template <VR vr>
    void put(Tag tag)
    {
      Value v(vr);
      insert(value_type(tag, v));
    }


  //!Get all values matching tag.
  /*!
  This helps the user avoid using the somewhat complex equal_range() interface.
  */

    std::vector<Value> values(const Tag tag) const
    {
      std::pair<const_iterator, const_iterator> P = equal_range(tag);

      std::vector<Value> v;
      for(const_iterator I = P.first; I != P.second; ++I)
      {
        v.push_back(I->second);
      }

      return v;
    }


  //!Check if a Tag exists
  /*!
  This helps the user avoid running into/dealing with too many exceptions.
  */

    bool exists(const Tag tag) const
    {
      return (find(tag) != end());
    }
  };

  typedef std::vector<DataSet> Sequence;
}//namespace DICOM
