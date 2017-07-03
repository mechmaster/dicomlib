#pragma once

#include <fstream>

#include "UIDs.hpp"
#include "Exceptions.hpp"
#include "FileMetaInformation.hpp"

namespace dicom
{
  /*
  What operations do we actually want to expose to the end user?
  Basically,
  Read from an istream onto a dataset.
  Write from a dataset onto an ostream, appending appropriate meta info at begin...

  so, maybe...
  ReadFromStream(istream,dataset);

  and then

  WriteToStream(dataset,ostream);

  all of which implies that File should be a hidden (implementation only) class.

  One reason for keeping it visible would be so that users could go:

  File f(...);
  modify(f.data);
  f.Write(...);

  Alternatively, we could bundle all this functionality into DataSet:

  DataSet ds(istream);
  ds.Write(ostream);

  and then maybe we wouldn't even have a file object.
  */

  //!Thrown if we can't open the file, it's corrupt.
  class FileException:public dicom::exception
  {

  public:

    FileException(std::string Description) :
      exception(Description)
    {
    }
  };

  bool ReadFileMetaFromStream(std::ifstream& In, DataSet& ds);

  void ReadFromStream(std::ifstream& In, DataSet& data, std::uint64_t max_number_of_byte_to_read = 0);

  void WriteToStream(const DataSet& data, std::ostream& Out, TS ts = TS(IMPL_VR_LE_TRANSFER_SYNTAX), bool Tiff = true);

  void Read(std::string FileName, DataSet& data, std::uint64_t max_number_of_byte_to_read = 0);

  //Note: default Tiff compatible. Still allow to write Pure Dicom when supply false
  //!mge @ May 2009
  void Write(const DataSet& data, std::string FileName, TS ts = TS(IMPL_VR_LE_TRANSFER_SYNTAX), bool Tiff = true);//why implicit?
}//namespace dicom

