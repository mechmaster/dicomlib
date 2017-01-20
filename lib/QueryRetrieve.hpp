#ifndef QUERY_RETRIEVE_HPP_INCLUDE_GUARD_U4DI92XIN23
#define QUERY_RETRIEVE_HPP_INCLUDE_GUARD_U4DI92XIN23
#include <string>
#include "Tag.hpp"
namespace dicom
{
	namespace QueryRetrieve
	{
		//!See Part 4, C.3.1
		enum Level
		{
			IMAGE,
			SERIES,
			STUDY,
			PATIENT,//Order them this way round so PATIENT>STUDY, etc...
		};

		//!converts a string to a level value.
		Level GetLevel(const std::string& level);
		//!looks up the level associated with the provided tag.
		Level GetLevel(Tag tag);

		//!Gets the tag for the unique identifier at this level
		Tag GetUIDTag(Level level);

		//!Converts a level value to a string.
		std::string ToString(Level level);

		//!Gets next level up
		Level ParentLevel(Level level);

		//!Gets next level down
		Level ChildLevel(Level level);

		//!See part 4, C.3.(1-3)
		enum Root
		{
			PATIENT_ROOT,
			STUDY_ROOT,
			PATIENT_STUDY_ONLY,
			MODALITY_WORKLIST, //K.6.1.4
			GENERAL_PURPOSE_WORKLIST //K.6.2.4
		};
	}//namespace QueryRetrieve


	std::ostream& operator <<  (std::ostream& out, dicom::QueryRetrieve::Level level);

}//namespace dicom




#endif //QUERY_RETRIEVE_HPP_INCLUDE_GUARD_U4DI92XIN23
