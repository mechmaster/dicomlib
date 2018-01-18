#include <map>

#include "Exceptions.hpp"
#include "QueryRetrieve.hpp"

namespace dicom
{
  namespace QueryRetrieve
  {
    Level GetLevel(const std::string& level)
    {
      static std::map<std::string, Level> stingToLevelList = 
      {
        {"PATIENT", Level::PATIENT},
        {"STUDY", Level::STUDY},
        {"SERIES", Level::SERIES},
        {"IMAGE", Level::IMAGE},
      };
      
      auto iter = stingToLevelList.find(level);
      if (iter == stingToLevelList.end())
      {
        throw exception("Unknown Query/Retrieve level specified.");
      }
      
      return iter->second;
    }

    Level ParentLevel(Level level)
    {
      switch (level)
      {
      case Level::STUDY:
        return Level::PATIENT;
      case Level::SERIES:
        return Level::STUDY;
      case Level::IMAGE:
        return Level::SERIES;
      default:
        throw exception("Invalid Query/Retrieve level value.");
      }
    }

    Level Child(Level level)
    {
      switch (level)
      {
      case Level::PATIENT:
        return Level::STUDY;
      case Level::STUDY:
        return Level::SERIES;
      case Level::SERIES:
        return Level::IMAGE;
      default:
        throw exception("Invalied Query/Retrieve level value.");
      }
    }

    std::string ToString(Level level)
    {
      switch (level)
      {
      case Level::PATIENT:
        return "PATIENT";
      case Level::STUDY:
        return "STUDY";
      case Level::SERIES:
        return "SERIES";
      case Level::IMAGE:
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
        return Level::PATIENT;
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
        return Level::STUDY;
      case TAG_MODALITY:
      case TAG_SERIES_NO:
      case TAG_SERIES_INST_UID:
        return Level::SERIES;
      default:
        return Level::IMAGE;
      }
    }

    Tag GetUIDTag(const Level level)
    {
      switch (level)
      {
      case Level::PATIENT:
        return TAG_PAT_ID;
      case Level::STUDY:
        return TAG_STUDY_INST_UID;
      case Level::SERIES:
        return TAG_SERIES_INST_UID;
      case Level::IMAGE:
        return TAG_SOP_INST_UID;
      default:
        throw exception("Invalid Query/Retrieve level value.");
      }
    }
  }//namespace QueryRetrieve

  //!should this be in the global namespace?
  std::ostream& operator << (std::ostream& out, QueryRetrieve::Level level)
  {
    out << QueryRetrieve::ToString(level);
    return out;
  }
}//namespace dicom
