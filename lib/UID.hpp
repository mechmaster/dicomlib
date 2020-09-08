#pragma once

#include <string>

#include "Types.hpp"
#include "Utility.hpp"
#include "Exceptions.hpp"

namespace dicom
{
  //!Thrown if construction of UID fails.
  class InvalidUID : public dicom::exception
  {
    
  public:
    
    InvalidUID(std::string what = "Invalid UID");

    virtual ~InvalidUID()
    {
    }
  };

  //! Unique Identifier class.
  /*!
  Specified in Part 8, Annex F.1 (page 49)

  A UID is basically a string of up to 64 characters, which can be the numbers 0-9 or a period ('.')
  */

  class UID
  {
    
  public:

    //!Verifying constructor
    UID(const std::string& s = "");

    //!Access underlying string representation.
    std::string str() const;

    //!So we can sort on UID
    bool operator < (const UID& comp) const;
    bool operator == (const UID& comp) const;
    bool operator !=(const UID& comp) const;

  private:

    std::string m_data;
  };


  //!Create a random, hopefully-unique UID with the supplied prefix.
  UID makeUID(const std::string& Prefix);
}//namespace dicom
