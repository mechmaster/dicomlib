#ifndef VIEW_CODE_SEQUENCE_HPP_INCLUDE_GUARD_5748380725
#define VIEW_CODE_SEQUENCE_HPP_INCLUDE_GUARD_5748380725

#include <string>

/**
*	switch between commonly used term such as "CC view" and dicom ViewCodeSequence
*	created by mge
*	modified by mge @ Jan 2009
*/
namespace dicom
{
	enum View 
	{
		ML,
		MLO, 
		LM, 
		LMO, 
		CC, 
		FB, 
		SIO, 
		XCC, 
		XCCL, 
		XCCM
	};

	struct ViewCodeSequenceEntry
	{
		std::string view_code_value;
		std::string view_coding_scheme_designator;
		std::string view_code_meaning;
		
		ViewCodeSequenceEntry( )
			:view_code_value("")
			,view_coding_scheme_designator("")
			,view_code_meaning("")
		{
		}


		ViewCodeSequenceEntry(std::string code_value,
								std::string coding_scheme_designator,
								std::string code_meaning)
		{
			view_code_value=code_value;
			view_coding_scheme_designator=coding_scheme_designator;
			view_code_meaning=code_meaning;
		}
	};


    void MakeViewCodeSequence(DataSet &data_set, View view);

	View GetView(ViewCodeSequenceEntry& entry);//Jan 2009

	dicom::View StringToDicomView(std::string str);//Jan 2009
	std::string DicomViewToString(View view);//Jan 2009

}//namespace

#endif   //VIEW_CODE_SEQUENCE_HPP_INCLUDE_GUARD_5748380725