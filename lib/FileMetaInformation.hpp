#pragma once

#include <iostream>
#include <array>

#include "DataSet.hpp"
#include "Exceptions.hpp"
#include "TransferSyntax.hpp"
#include "ImplementationUID.hpp"

namespace dicom
{
  struct FileMetaInfoException : public dicom::exception
  {
    FileMetaInfoException(const std::string& description) : exception(description)
    {
    }
  };

  //!DICOM File Meta Information
  /*!
  Documented in Part 10, Section 7.1

  Consists of:
  Preamble (128 bytes)
  DICOM prefix (4 bytes)
  File Meta Elements (Part 10, table 7.1-1)
  */

  class FileMetaInformation
  {
    std::array<std::uint8_t, 128> m_preamble;
    
    DataSet m_metaElements;

  public:

    /*
    What constructors are we going to allow?
    -Read _from_ a stream
    -construct with the intention of writing _to_ a stream.
    */

    FileMetaInformation(std::istream& In);

    //!Infer instance and class UIDs from data set
    FileMetaInformation(const DataSet& data, TS ts);

    std::uint32_t write(std::ostream& Out);
  };
}//namespace dicom
