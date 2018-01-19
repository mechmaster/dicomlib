/************************************************************************
* DICOMLIB
* Copyright 2003 Sunnybrook and Women's College Health Science Center
* Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
* See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <sstream>

#include "Tag.hpp"
#include "DataDictionary.hpp"

namespace dicom
{
  const char* TagNotFound::what() const noexcept
  {
    std::ostringstream os;
    os << "Tag Not Found: " << GetName(m_tag);
    return os.str().c_str();
  }
}
