#include "QueryRetrieve.hpp"
#include "Exceptions.hpp"

namespace dicom
{
	namespace QueryRetrieve
	{
		Level GetLevel(const std::string& level)
		{
			if(level=="PATIENT")
				return PATIENT;
			if(level=="STUDY")
				return STUDY;
			if(level=="SERIES")
				return SERIES;
			if(level=="IMAGE")
				return IMAGE;
			throw exception("Unknown Query/Retrieve level specified.");
		}
		Level ParentLevel(Level level)
		{
			switch (level)
			{
				case STUDY:
					return PATIENT;
				case SERIES:
					return STUDY;
				case IMAGE:
					return SERIES;
				default:
					throw exception("Invalid Query/Retrieve level value.");
			}
		}
		Level Child(Level level)
		{
			switch (level)
			{
				case PATIENT:
					return STUDY;
				case STUDY:
					return SERIES;
				case SERIES:
					return IMAGE;
				default:
					throw exception("Invalied Query/Retrieve level value.");
			}
		}

		std::string ToString(QueryRetrieve::Level level)
		{
			using namespace QueryRetrieve;
			switch (level)
			{
				case PATIENT:
					return "PATIENT";
				case STUDY:
					return "STUDY";
				case SERIES:
					return "SERIES";
				case IMAGE:
					return "IMAGE";
				default:
					throw exception("Unknown Query/Retrieve level value.");
			}
		}


		/*!
			Are we sure about this list? Is it dependent
			on QR model?
		*/

		Level GetLevel(Tag tag)
		{
			switch (tag)
			{
				case TAG_PAT_NAME:
				case TAG_PAT_ID:
				case TAG_PAT_BIRTH_DATE:
				case TAG_PAT_SEX:
				case TAG_OTHER_PAT_ID:
				case TAG_PAT_COMMENT:
					return PATIENT;
				case TAG_STUDY_DATE:
				case TAG_STUDY_TIME:
				case TAG_ACCESS_NO:
				case TAG_STUDY_ID:
				case TAG_STUDY_INST_UID:
				case TAG_REF_PHYS_NAME:
				case TAG_STUDY_DESC:
				case TAG_READ_PHYS_NAME:
				case TAG_ADMIT_DIAG_DESC:
				case TAG_PAT_AGE:
				case TAG_PAT_SIZE:
				case TAG_PAT_WEIGHT:
				case TAG_INTERPRET_AUTHOR:
					return STUDY;
				case TAG_MODALITY:
				case TAG_SERIES_NO:
				case TAG_SERIES_INST_UID:
					return SERIES;
				default:
					return IMAGE;
			}
		}
		Tag GetUIDTag(const Level level)
		{
			switch (level)
			{
				case PATIENT:
					return TAG_PAT_ID;
				case STUDY:
					return TAG_STUDY_INST_UID;
				case SERIES:
					return TAG_SERIES_INST_UID;
				case IMAGE:
					return TAG_SOP_INST_UID;
				default:
					throw exception("Invalid Query/Retrieve level value.");
			}
		}
	}//namespace QueryRetrieve

	//!should this be in the global namespace?
	std::ostream& operator <<  (std::ostream& out, QueryRetrieve::Level level)
	{
		out << QueryRetrieve::ToString(level);
		return out;
	}
}//namespace dicom
