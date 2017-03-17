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

const BYTE m_result;
const BYTE m_source;
const BYTE m_reason;

AssociationRejection(BYTE Result, BYTE Source, BYTE Reason);

std::string GetResult() const;
std::string GetSource() const;
std::string GetReason() const;

};
}//namespace dicom
