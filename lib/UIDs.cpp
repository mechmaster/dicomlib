#include "UIDs.hpp"
#include<map>
using namespace std;
namespace dicom
{
	namespace
	{

		//!Construct a map of human-readable names for each SOP class.

		void PopulateMap(map<UID,string>& THEMAP)
		{
			THEMAP[CR_IMAGE_STORAGE_SOP_CLASS]="Computed Radiography Image Storage";
			THEMAP[CT_IMAGE_STORAGE_SOP_CLASS]="CT Image Storage";
			THEMAP[USOLD_MF_IMAGE_STORAGE_SOP_CLASS]="Ultrasound Multi-frame Image Storage (Retired)";
			THEMAP[US_MF_IMAGE_STORAGE_SOP_CLASS]="Ultrasound Multi-frame Image Storage";
			THEMAP[MR_IMAGE_STORAGE_SOP_CLASS]="MR Image Storage";
			THEMAP[USOLD_IMAGE_STORAGE_SOP_CLASS]="Ultrasound Image Storage (Retired)";
			THEMAP[US_IMAGE_STORAGE_SOP_CLASS]="Ultrasound Image Storage";
			THEMAP[SC_IMAGE_STORAGE_SOP_CLASS]="Secondary Capture Image Storage";
			THEMAP[NM_IMAGE_STORAGE_SOP_CLASS]="Nuclear Medicine Image Storage";
			THEMAP[XA_IMAGE_STORAGE_SOP_CLASS]="X-Ray Angiographic Image Storage";
			THEMAP[XRF_IMAGE_STORAGE_SOP_CLASS]="X-Ray Radiofluoroscopic Image Storage";
			THEMAP[XA2_IMAGE_STORAGE_SOP_CLASS]="X-Ray Angiographic Bi-Plane Image Storage (Retired)";
			THEMAP[PET_IMAGE_STORAGE_SOP_CLASS]="Positron Emission Tomography Image Storage";
			THEMAP[DX_PRES_IMAGE_STORAGE_SOP_CLASS]="Digital X-Ray Image Storage - For Presentation";
			THEMAP[DX_PROC_IMAGE_STORAGE_SOP_CLASS]="Digital X-Ray Image Storage - For Processing";
			THEMAP[MAMMO_PRES_IMAGE_STORAGE_SOP_CLASS]="Digital Mammography X-Ray Image Storage - For Presentation";
			THEMAP[MAMMO_PROC_IMAGE_STORAGE_SOP_CLASS]="Digital Mammography X-Ray Image Storage - For Processing";
			THEMAP[INTRAORAL_PRES_IMAGE_STORAGE_SOP_CLASS]="Digital Intra-oral X-Ray Image Storage - For Presentation";
			THEMAP[INTRAORAL_PROC_IMAGE_STORAGE_SOP_CLASS]="Digital Intra-oral X-Ray Image Storage - For Processing";
			THEMAP[BASIC_TEXT_SR_STORAGE_SOP_CLASS]="Basic Text Structured Report Storage";
			THEMAP[ENHANCED_SR_STORAGE_SOP_CLASS]="Enhanced Structured Report Storage";
			THEMAP[COMPREHENSIVE_SR_STORAGE_SOP_CLASS]="Comprehensive Structured Report Storage";
		}

		map<UID,string>THEMAP;
		bool MapIsPopulated=false;
	}//namespace

	string GetUIDName(UID uid)
	{
		if(!MapIsPopulated)
			PopulateMap(THEMAP);//threading issues?
		if(THEMAP.find(uid)!=THEMAP.end())
			return THEMAP[uid];
		else
			return "Unknown";
	}
}
