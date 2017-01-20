#include <iostream>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <boost/tokenizer.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/date_parsing.hpp>

#include "DataSet.hpp"
//#include "Exceptions.hpp"
//#include "DataDictionary.hpp"
#include "ViewCodeSequence.hpp"


namespace dicom{


	void MakeViewCodeSequence(DataSet &data_set, View view)
	{
		Sequence view_seq;
		DataSet view_ds;
		ViewCodeSequenceEntry entry;
         
		switch(view)
		{
			case ML:
					entry=ViewCodeSequenceEntry("R-10224","SNM3","medio-lateral");
					break;
			case MLO:
					entry=ViewCodeSequenceEntry("R-10226","SNM3","medio-lateral oblique");
					break;
			case LM:
					entry=ViewCodeSequenceEntry("R-10228","SNM3","latero-medial");
					break;
			case LMO:
					entry=ViewCodeSequenceEntry("R-10230","SNM3","lateral-medio oblique");
					break;
			case CC:
					//entry.view_code_value = "R-10242";
					//entry.view_coding_scheme_designator = "SNM3";
					//entry.view_code_meaning = "cranio-caudal";
					entry=ViewCodeSequenceEntry("R-10242","SNM3","cranio-caudal");
					break;
			case FB:
					entry=ViewCodeSequenceEntry("R-10244","SNM3","cranio-caudal (from below)");
					break;
			case SIO:
					entry=ViewCodeSequenceEntry("R-102D0","SNM3","superlateral to inferomedial oblique");
					break;
			case XCC:
					entry=ViewCodeSequenceEntry("R-102CF","SNM3","exaggerated cranio-caudal");
					break;
			case XCCL:
					entry=ViewCodeSequenceEntry("R-1024A","SRT","cranio-caudal exaggerated laterally");
					break;
			case XCCM:
					entry=ViewCodeSequenceEntry("R-1024B","SRT","cranio-caudal exaggerated medially");
					break;
			default:
					throw dicom::exception("Unknown view type specified.");
		}

		//populate dataset
		view_ds.Put<VR_SH>(TAG_CODE_VALUE, entry.view_code_value);//T1
		view_ds.Put<VR_SH>(TAG_CODING_SCHEME_DESIGNATOR, entry.view_coding_scheme_designator);//T1
		view_ds.Put<VR_SH>(TAG_CODE_MEANING, entry.view_code_meaning);//T1

		//make empty view modifier sequence
		dicom::Sequence view_modifier_seq;
		dicom::DataSet view_modifier_ds;
		view_modifier_seq.push_back(view_modifier_ds);
		view_ds.Put<VR_SQ>(makeTag(0x0054, 0x0222), view_modifier_seq);


		view_seq.push_back(view_ds);

		////make empty view modifier sequence
		//dicom::Sequence view_modifier_seq;
		//dicom::DataSet view_modifier_ds;
		//view_modifier_seq.push_back(view_modifier_ds);
		//view_ds.Put<VR_SQ>(makeTag(0x0054, 0x0222), view_modifier_seq);

		data_set.erase(makeTag(0x0054, 0x0220));
		data_set.Put<VR_SQ>(makeTag(0x0054, 0x0220),view_seq);
	}


	View GetView(ViewCodeSequenceEntry& entry)
	{
		dicom::View view;

		if(entry.view_code_value == "R-10224")
			view = ML;
		else if(entry.view_code_value == "R-10226")
			view = MLO;
		else if(entry.view_code_value == "R-10228")
			view = LM;
		else if(entry.view_code_value == "R-10230")
			view = LMO;
		else if(entry.view_code_value == "R-10242")
			view = CC;
		else if(entry.view_code_value == "R-10244")
			view = FB;
		else if(entry.view_code_value == "R-102D0")
			view = SIO;
		else if(entry.view_code_value == "R-102CF")
			view = XCC;
		else if(entry.view_code_value == "R-1024A")
			view = XCCL;
		else if(entry.view_code_value == "R-1024B")
			view = XCCM;
		else
			throw dicom::exception("Unknown view type specified.");

		return view;
	}

	dicom::View StringToDicomView(std::string str)
	{
		transform(str.begin(), str.end(), str.begin(), toupper);

		dicom::View dicom_view;

		if(str=="CC")
			dicom_view=dicom::CC;
		else if(str=="ML")
			dicom_view=dicom::ML;
		else if(str=="MLO")
			dicom_view=dicom::MLO;
		else if(str=="LM")
			dicom_view=dicom::LM;
		else if(str=="LMO")
			dicom_view=dicom::LMO;
		else if(str=="FB")
			dicom_view=dicom::FB;
		else if(str=="SIO")
			dicom_view=dicom::SIO;
		else if(str=="XCC")
			dicom_view=dicom::XCC;
		else if(str=="XCCL")
			dicom_view=dicom::XCCL;
		else if(str=="XCCM")
			dicom_view=dicom::XCCM;
		else
			throw dicom::exception("Unknown view type specified.");

		return dicom_view;
	}

	std::string DicomViewToString(View view)
	{
		std::string vstr;
         
		switch(view)
		{
			case ML:
					vstr = "ML";
					break;
			case MLO:
					vstr = "MLO";
					break;
			case LM:
					vstr = "LM";
					break;
			case LMO:
					vstr = "LMO";
					break;
			case CC:
					vstr = "CC";
					break;
			case FB:
					vstr = "FB";
					break;
			case SIO:
					vstr = "SIO";
					break;
			case XCC:
					vstr = "XCC";
					break;
			case XCCL:
					vstr = "XCCL";
					break;
			case XCCM:
					vstr = "XCCM";
					break;
			default:
					throw dicom::exception("Unknown view type specified.");
		}

		return vstr;
	}
}
