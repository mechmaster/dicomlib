#pragma once

#include <string>
#include <ostream>
#include <istream>

#include "VR.hpp"
#include "Value.hpp"

namespace dicom
{
  //!pipes a string representation of value onto the stream.
  std::ostream& operator <<  (std::ostream& out, const dicom::Value& value);

  //!Create a value from a valid stream.
  Value ValueFromStream(VR vr, std::istream& In);

  //!Just give the value data in string
  std::string GetValueDataInString(const Value& value);

  //!Get a human readable name for a given VR
  std::string GetVRName(VR vr);//

  //!Converts e.g. VR_US to "US"
  std::string GetVRShortName(VR vr);

  //!Converts e.g. "US" to VR_US
  VR GetVRFromShortName(std::string strVR);

  //!pipes short name onto stream
  std::ostream& operator << (std::ostream& out, VR vr);
}
