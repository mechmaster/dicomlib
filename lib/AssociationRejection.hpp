#pragma once

#include "Types.hpp"
#include "Exceptions.hpp"

namespace dicom
{
//!Intended to be thrown on failed association.
/*!
This class stores the error fields passed in
an A-ASSOCIATE-RJ message, and provides human-
readable error strings. 

See  Part 8, table 9-21 for further documentation.
*/

class AssociationRejection : public dicom::exception
{
public:

  const std::uint8_t m_result;
  const std::uint8_t m_source;
  const std::uint8_t m_reason;

  AssociationRejection(std::uint8_t Result, std::uint8_t Source, std::uint8_t Reason);

  std::string GetResult() const;
  std::string GetSource() const;
  std::string GetReason() const;
};
}//namespace dicom
