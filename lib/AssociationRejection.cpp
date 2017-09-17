#include "AssociationRejection.hpp"

namespace dicom
{
  /*
  exception class.
  */

  AssociationRejection::AssociationRejection(std::uint8_t Result, std::uint8_t Source, std::uint8_t Reason) :
    dicom::exception("Association Rejection"),
    m_result(Result),
    m_source(Source),
    m_reason(Reason)
  {
  }

  std::string AssociationRejection::GetResult() const 
  {
    switch (m_result)
    {
      case 0x01:
        return "Rejected Permanent";
      case 0x02:
        return "Rejected Transient";
      default:
        return "Undefined";
    }
  }

  std::string AssociationRejection::GetSource() const
  {
    switch (m_source)
    {
      case 0x01:
        return "DICOM UL service-user";
      case 0x02:
        return "DICOM UL service-provider (ACSE related function)";
      case 0x03:
        return "DICOM UL service-provider (Presentation related function)";
      default:
        return "Undefined";
    }
  }

  std::string AssociationRejection::GetReason() const
  {
    switch (m_source)
    {
      case 0x01:
        switch (m_reason)
        {
          case 0x01:
            return "No reason given.";
          case 0x02:
            return "Application Context not supported.";
          case 0x03:
            return "Calling AE not recognized";
          case 0x07:
            return "Protocol version not supported";
        }
      case 0x02:
        switch(m_reason)
        {
          case 0x02:
          return "Protocol Version Not Supported.";
        }
      case 0x03:
        switch (m_reason)
        {
          case 0x01:
            return ("Temporary Congestion");
          case 0x02:
            return ("Local Limit Exceeded.");
        }
    }

    return "Unknown reason";
  }
}//namespace dicom
