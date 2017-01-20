#ifndef HPP_INCLUDE_GUARD_57343948
#define HPP_INCLUDE_GUARD_57343948

#include <string>
#include "UID.hpp"
namespace dicom
{
	/*
		Should the following constants be strings or UIDs?
	*/

	/*
		Should these be in a sub-namespace?
	*/



	/*
		transfer syntaxes
	*/
	//!Implicit VR, Little Endian
	const UID IMPL_VR_LE_TRANSFER_SYNTAX			= UID("1.2.840.10008.1.2");

	//!Explicit VR, Little Endian
	const UID EXPL_VR_LE_TRANSFER_SYNTAX			= UID("1.2.840.10008.1.2.1");

	//!Not sure what this is!  Probably shouldn't advertise as supporting it!
	const UID DEFLATED_EXPL_VR_LE_TRANSFER_SYNTAX   = UID("1.2.840.10008.1.2.1.99");

	//!Explicit VR, Big Endian
	const UID EXPL_VR_BE_TRANSFER_SYNTAX			= UID("1.2.840.10008.1.2.2");


	//JPEG encoding:

	//!Baseline JPEG Compression (coding Process 1)
	/*!
		This must be supported by an implementation that supports any 8-bit lossy compression.
		See Part 5 Section 8.2.1
	*/
	const UID JPEG_BASELINE_TRANSFER_SYNTAX 		= UID("1.2.840.10008.1.2.4.50");

	//!Lossless JPEG compression (coding process 14, first-order horizontal prediction)
	/*!
		This must be supported by an implementation that supports any lossless compression.
		See Part 5 Section 8.2.1
	*/
		//has to be supported if we do any lossless jpeg, see note 
	const UID JPEG_LOSSLESS_NON_HIERARCHICAL		= UID("1.2.840.10008.1.2.4.70");

	//!JPEG2000LosslessOnly (Part5 Annex A.4.4, first type
	const UID JPEG2000_LOSSLESS_ONLY				= UID("1.2.840.10008.1.2.4.90");

	/*
		There are more to still go in here, mostly to do with JPEG encoding.
		These are in the range 1.2.840.10008.1.2.4.50 to 1.2.840.10008.1.2.4.70

		Part 5, Annex A.4 says that the entire dataset in these cases is encoded
		Little Endian, Explicit VR, and then provides descriptions of how pixel
		data is to be encoded.  This is outside the scope of this library,
		but we should at least recognize the transfer syntaxes

	*/

    const UID JPEG2000                              = UID("1.2.840.10008.1.2.4.91");



	/*
		End of transfer syntaxes
	*/

	const UID MEDIA_DIR_STORAGE_SOP_CLASS			= UID("1.2.840.10008.1.3.10");

	//!Part 7, Annex A.21 says that this is the ONLY application context name
	const UID APPLICATION_CONTEXT					= UID("1.2.840.10008.3.1.1.1");

	const UID MODALITY_PPS_SOP_CLASS				= UID("1.2.840.10008.3.1.2.3.3");


	//!Where is this defined?
	const UID VERIFICATION_SOP_CLASS	        	= UID("1.2.840.10008.1.1");


	/*
	*	Following are defined in Part 4, Table B.5-1
	*/
	const UID CR_IMAGE_STORAGE_SOP_CLASS	    		= UID("1.2.840.10008.5.1.4.1.1.1");
	const UID DX_PRES_IMAGE_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.1.1");
	const UID DX_PROC_IMAGE_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.1.1.1");
	const UID MAMMO_PRES_IMAGE_STORAGE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.1.1.2");
	const UID MAMMO_PROC_IMAGE_STORAGE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.1.1.2.1");
	const UID INTRAORAL_PRES_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.1.3");
	const UID INTRAORAL_PROC_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.1.3.1");
	const UID CT_IMAGE_STORAGE_SOP_CLASS	    		= UID("1.2.840.10008.5.1.4.1.1.2");
	const UID ENHANCED_CT_IMAGE_STORAGE_SOP_CLASS	    = UID("1.2.840.10008.5.1.4.1.1.2.1");
	const UID USOLD_MF_IMAGE_STORAGE_SOP_CLASS  		= UID("1.2.840.10008.5.1.4.1.1.3");//retired
	const UID US_MF_IMAGE_STORAGE_SOP_CLASS  			= UID("1.2.840.10008.5.1.4.1.1.3.1");
	const UID MR_IMAGE_STORAGE_SOP_CLASS	    		= UID("1.2.840.10008.5.1.4.1.1.4");
	const UID ENHANCED_MR_IMAGE_STORAGE_SOP_CLASS	    = UID("1.2.840.10008.5.1.4.1.1.4.1");
	const UID MR_SPECTROSCOPY_STORAGE_SOP_CLASS	    	= UID("1.2.840.10008.5.1.4.1.1.4.2");
	const UID USOLD_IMAGE_STORAGE_SOP_CLASS				= UID("1.2.840.10008.5.1.4.1.1.6");//retired
	const UID US_IMAGE_STORAGE_SOP_CLASS			    = UID("1.2.840.10008.5.1.4.1.1.6.1");
	const UID SC_IMAGE_STORAGE_SOP_CLASS			    = UID("1.2.840.10008.5.1.4.1.1.7");
	const UID MF_SB_SC_IMAGE_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.7.1");
	const UID MF_GREYBITE_SC_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.7.2");
	const UID MF_GREYWORD_SC_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.7.3");
	const UID MF_TRUECOLOR_SC_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.7.4");
	//lots of Waveform not implemented here
	const UID XA_IMAGE_STORAGE_SOP_CLASS			    = UID("1.2.840.10008.5.1.4.1.1.12.1");
	const UID ENHANCED_XA_IMAGE_STORAGE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.1.12.1.1");
	const UID XRF_IMAGE_STORAGE_SOP_CLASS				= UID("1.2.840.10008.5.1.4.1.1.12.2");
	const UID ENHANCED_XRF_IMAGE_STORAGE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.1.12.2.1");
	const UID XA2_IMAGE_STORAGE_SOP_CLASS				= UID("1.2.840.10008.5.1.4.1.1.12.3");//retired
	const UID XA_3D_IMAGE_STORAGE_SOP_CLASS			    = UID("1.2.840.10008.5.1.4.1.1.13.1.1");
	const UID XC_3D_IMAGE_STORAGE_SOP_CLASS			    = UID("1.2.840.10008.5.1.4.1.1.13.1.2");
	const UID NM_IMAGE_STORAGE_SOP_CLASS		    	= UID("1.2.840.10008.5.1.4.1.1.20");
	const UID RAW_DATA_STORAGE_SOP_CLASS			    = UID("1.2.840.10008.5.1.4.1.1.66");
	const UID SPATIAL_REG_STORAGE_SOP_CLASS			    = UID("1.2.840.10008.5.1.4.1.1.66.1");
	const UID SPATIAL_FID_STORAGE_SOP_CLASS			    = UID("1.2.840.10008.5.1.4.1.1.66.2");
	const UID DEFORM_SPATIAL_REG_STORAGE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.1.66.3");
	const UID SEGMENTATION_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.66.4");
	const UID REAL_WORLD_VALUE_MAP_STORAGE_SOP_CLASS    = UID("1.2.840.10008.5.1.4.1.1.67");
	const UID VL_END_IMAGE_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.77.1.1");
	const UID VIDEO_END_IMAGE_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.77.1.1.1");
	const UID VL_MICROSCOPIC_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.77.1.2");
	const UID VIDEO_MICROSCOPIC_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.77.1.2.1");
	const UID VL_SC_MICROSCOPIC_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.77.1.3");
	const UID VL_PHOTO_IMAGE_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.77.1.4");
	const UID VIDEO_PHOTO_IMAGE_STORAGE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.1.77.1.4.1");
	const UID OPHTHALMIC_PHOTO8_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.77.1.5.1");
	const UID OPHTHALMIC_PHOTO16_IMAGE_STORAGE_SOP_CLASS= UID("1.2.840.10008.5.1.4.1.1.77.1.5.2");
	const UID STEREOMETRIC_RELATION_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.77.1.5.3");
	const UID OPHTHALMIC_TOMO_IMAGE_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.77.1.5.4");
	const UID BASIC_TEXT_SR_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.88.11");
	const UID ENHANCED_SR_STORAGE_SOP_CLASS				= UID("1.2.840.10008.5.1.4.1.1.88.22");
	const UID COMPREHENSIVE_SR_STORAGE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.1.88.33");
	const UID PROCEDURE_LOG_STORAGE_SOP_CLASS		    = UID("1.2.840.10008.5.1.4.1.1.88.40");
	const UID MAMMO_CAD_SR_STORAGE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.1.88.50");
	const UID KEY_OBJ_SELECTION_SOP_CLASS		        = UID("1.2.840.10008.5.1.4.1.1.88.59");
	const UID CHEST_CAD_SR_SOP_CLASS		            = UID("1.2.840.10008.5.1.4.1.1.88.65");
	const UID XRAY_RADIATION_DOSE_SR_SOP_CLASS		    = UID("1.2.840.10008.5.1.4.1.1.88.67");
	const UID PET_IMAGE_STORAGE_SOP_CLASS				= UID("1.2.840.10008.5.1.4.1.1.128");
	const UID RT_IMAGE_STORAGE_SOP_CLASS		        = UID("1.2.840.10008.5.1.4.1.1.481.1");
	const UID RT_DOSE_STORAGE_SOP_CLASS		            = UID("1.2.840.10008.5.1.4.1.1.481.2");
	const UID RT_STRUCTURE_SET_STORAGE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.1.481.3");
	const UID RT_BEAM_TREATMENT_RC_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.481.4");
	const UID RT_PLAN_STORAGE_SOP_CLASS		            = UID("1.2.840.10008.5.1.4.1.1.481.5");
	const UID RT_BRACHY_TREATMENT_RC_STORAGE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.1.481.6");
	const UID RT_TREATMENT_SUMMARY_RC_STORAGE_SOP_CLASS = UID("1.2.840.10008.5.1.4.1.1.481.7");
	const UID RT_ION_PLAN_STORAGE_SOP_CLASS	            = UID("1.2.840.10008.5.1.4.1.1.481.8");
	const UID RT_ION_BEAM_TREATMENT_RC_STORAGE_SOP_CLASS= UID("1.2.840.10008.5.1.4.1.1.481.9");
	const UID GRAYSCALE_SOFTCOPY_PS_STORAGE_SOP_CLASS   = UID("1.2.840.10008.5.1.4.1.1.11.1");

	//!Feed in one of the above entries and get a human-readable string in return.
	std::string GetUIDName(UID StorageSOP);

	/*
		See Part 4, Section C.3.1

		Defined in Part 4, Sections C.6.1.3, C.6.2.3-1
	*/
	const UID PATIENT_ROOT_QR_FIND_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.2.1.1");
	const UID PATIENT_ROOT_QR_MOVE_SOP_CLASS		= UID("1.2.840.10008.5.1.4.1.2.1.2");
	const UID PATIENT_ROOT_QR_GET_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.2.1.3");

	const UID STUDY_ROOT_QR_FIND_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.2.2.1");
	const UID STUDY_ROOT_QR_MOVE_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.2.2.2");
	const UID STUDY_ROOT_QR_GET_SOP_CLASS			= UID("1.2.840.10008.5.1.4.1.2.2.3");

	//Retired in PS3.4 2008
	const UID PATIENT_STUDY_ONLY_QR_FIND_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.2.3.1");
	const UID PATIENT_STUDY_ONLY_QR_MOVE_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.2.3.2");
	const UID PATIENT_STUDY_ONLY_QR_GET_SOP_CLASS	= UID("1.2.840.10008.5.1.4.1.2.3.3");

	const UID MODALITY_WORKLIST_SOP_CLASS			= UID("1.2.840.10008.5.1.4.31");
	const UID GENERAL_PURPOSE_WORKLIST_SOP_CLASS	= UID("1.2.840.10008.5.1.4.32.1");
}//namespace dicom
#endif //HPP_INCLUDE_GUARD_57343948
