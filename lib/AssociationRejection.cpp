#include "AssociationRejection.hpp"
namespace dicom
{


	/*
		exception class.
	*/

	AssociationRejection::AssociationRejection(BYTE Result, BYTE Source, BYTE Reason)
	:
		dicom::exception("Association Rejection"),
		Result_(Result),
		Source_(Source),
		Reason_(Reason)
	{

	}
	std::string AssociationRejection::GetResult() const 
	{
		switch (Result_)
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
		switch(Source_)
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
		switch(Source_)
		{
		case 0x01:
			switch(Reason_)
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
			switch(Reason_)
			{
				case 0x02:
					return "Protocol Version Not Supported.";
			}

		case 0x03:
			switch(Reason_)
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
