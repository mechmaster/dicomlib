#pragma once

#include <iostream>
#include <ostream>

#include "DataSet.hpp"

namespace dicom
{
  //!Prints the contents of 'data' to the provided output stream.
  void Dump(const DataSet& data, std::ostream& Out = std::cout);

  //!operator does the same as Dump()
  std::ostream& operator << (std::ostream& out, const dicom::DataSet& data);
}
