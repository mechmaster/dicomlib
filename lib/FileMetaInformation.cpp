/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "FileMetaInformation.hpp"
#include "TransferSyntax.hpp"
#include "Encoder.hpp"
#include "Buffer.hpp"
#include "Decoder.hpp"
#include "Encoder.hpp"
#include "GroupLength.hpp"
#include "UIDs.hpp"

//#include "ImageDisplay/TiffFile.hpp"


namespace dicom
{
// 	//!Figure out the length in bytes of a dataset by writing it to a temporary buffer.
// 	/*!
// 		This is probably an inefficient way of doing this, but the
// 		meta-info will generally be only of the order of a couple
// 		of hundred bytes at most, so I don't think this will have
// 		much impact on performance.
//
// 		Note that the group length is dependent on which transfer syntax we use, as
// 		explicit vr writes more info than implicit vr.
//
// 		This is actually needed in more places than here...
// 	*/
//
// 	UINT32 GetGroupLength(DataSet& data,TS ts)
// 	{
// 		int ByteOrder=ts.isBigEndian()?
// 			__BIG_ENDIAN:__LITTLE_ENDIAN;
//
// 		Buffer buffer(ByteOrder);
// 		//Encoder E(buffer,dataset,ts);
// 		//E.Encode();
// 		WriteToBuffer(data,buffer,ts);
// 		return (buffer.size());
// 	}

	void VerifyAcceptable(Tag tag,UID uid)
	{
		//verification code goes here.
	}


	/*!
		Refer to Part 10, Section 7.1 for a description of the fields here.
	*/

	FileMetaInformation::FileMetaInformation(const DataSet& data,TS ts)
	{
		const UID& classUID=data(TAG_SOP_CLASS_UID).Get<UID>();
		const UID& instUID=data(TAG_SOP_INST_UID).Get<UID>();
		VerifyAcceptable(TAG_MEDIA_SOP_CLASS_UID,classUID);//table 7.1-1 of part 10 implies some checking should occur here...

		std::fill(Preamble_,Preamble_+128,0);

		TypeFromVR<VR_OB>::Type VersionInfoData;//Set to (0,1) for the version of the standard that we support.
		VersionInfoData.push_back(0x00);
		VersionInfoData.push_back(0x01);
		MetaElements_.put<VR_OB>(TAG_FILE_INFO_VERS,VersionInfoData);

		MetaElements_.put<VR_UI>(TAG_MEDIA_SOP_CLASS_UID,classUID);
		MetaElements_.put<VR_UI>(TAG_MEDIA_SOP_INST_UID,instUID);
		MetaElements_.put<VR_UI>(TAG_TRANSFER_SYNTAX_UID,ts.getUID());
		MetaElements_.put<VR_UI>(TAG_IMPL_CLASS_UID,UID(ImplementationClassUID));
		MetaElements_.put<VR_SH>(TAG_IMPL_VERS_NAME,ImplementationVersionName);
		//MetaElements_.Put<VR_AE>(TAG_SRC_AET,SourceApplicationTitle);//this is optional
		UINT32 group_length=GroupLength(MetaElements_,TS(EXPL_VR_LE_TRANSFER_SYNTAX/*TS::EXPL_VR_LE*/));

		MetaElements_.put<VR_UL> (TAG_FILE_INFO_GR_LEN,group_length);//because multimap is sorted, this will
																	//get inserted at the front of MetaElements_

		if(MetaElements_.find(TAG_FILE_INFO_GR_LEN)!=MetaElements_.begin())
			throw exception("TAG_FILE_INFO_GR_LEN not at begin.");//test above assertion!

	}


/*
	void MakePreampleTiffHeader(const DataSet& data, long BytesToWrite, char* Preamble)
	{
		unsigned short width, height, samples_per_px, bits_allocated;

		data(dicom::TAG_COLUMNS) >> width;
		data(dicom::TAG_ROWS) >> height;
		data(TAG_SAMPLES_PER_PX) >> samples_per_px;
		data(TAG_BITS_ALLOC) >> bits_allocated;	

		ImageDisplay::TiffFileHeader tiff_header_(width, height, samples_per_px, bits_allocated, BytesToWrite);

		for(int i = 0; i<128; i++)
			Preamble[i] = tiff_header_.buf[i];
	}

*/
	FileMetaInformation::FileMetaInformation(const DataSet& data,TS ts,long BytesToWrite)
	{
		const UID& classUID=data(TAG_SOP_CLASS_UID).Get<UID>();
		const UID& instUID=data(TAG_SOP_INST_UID).Get<UID>();
		VerifyAcceptable(TAG_MEDIA_SOP_CLASS_UID,classUID);//table 7.1-1 of part 10 implies some checking should occur here...

		std::fill(Preamble_,Preamble_+128,0);

		//Adding Tiff metaheader
		//MakePreampleTiffHeader(data, BytesToWrite, Preamble_);

		TypeFromVR<VR_OB>::Type VersionInfoData;//Set to (0,1) for the version of the standard that we support.
		VersionInfoData.push_back(0x00);
		VersionInfoData.push_back(0x01);
		MetaElements_.put<VR_OB>(TAG_FILE_INFO_VERS,VersionInfoData);

		MetaElements_.put<VR_UI>(TAG_MEDIA_SOP_CLASS_UID,classUID);
		MetaElements_.put<VR_UI>(TAG_MEDIA_SOP_INST_UID,instUID);
		MetaElements_.put<VR_UI>(TAG_TRANSFER_SYNTAX_UID,ts.getUID());
		MetaElements_.put<VR_UI>(TAG_IMPL_CLASS_UID,UID(ImplementationClassUID));
		MetaElements_.put<VR_SH>(TAG_IMPL_VERS_NAME,ImplementationVersionName);
		//MetaElements_.Put<VR_AE>(TAG_SRC_AET,SourceApplicationTitle);//this is optional
		UINT32 group_length=GroupLength(MetaElements_,TS(EXPL_VR_LE_TRANSFER_SYNTAX/*TS::EXPL_VR_LE*/));

		MetaElements_.put<VR_UL> (TAG_FILE_INFO_GR_LEN,group_length);//because multimap is sorted, this will
																	//get inserted at the front of MetaElements_

		if(MetaElements_.find(TAG_FILE_INFO_GR_LEN)!=MetaElements_.begin())
			throw exception("TAG_FILE_INFO_GR_LEN not at begin.");//test above assertion!

	}


	FileMetaInformation::FileMetaInformation(std::istream& In)
	{
		if(!In)
			throw FileMetaInfoException("Input stream not open.");
		if(!In.seekg(0))
			throw FileMetaInfoException("Couldn't find beginning of stream.");
		if(!In.read(Preamble_,128))
			throw FileMetaInfoException("Couldn't read preamble.");

		char Prefix[4];
		if(!In.read(Prefix,4))
			throw FileMetaInfoException("Couldn't read prefix.");
		if(std::string(Prefix,Prefix+4) !="DICM")
			throw FileMetaInfoException("Prefix is not 'DICM'.");


		Buffer buffer(__LITTLE_ENDIAN);//Section 7.1 says this has to be used.

		//we know the length of this element...
		for(int I=0;I<12;I++)
			buffer.push_back(In.get());//This seems pretty inefficient...

		TS ts(EXPL_VR_LE_TRANSFER_SYNTAX/*TS::EXPL_VR_LE*/); //Section 7.1 says this has to be used.

		ReadElementFromBuffer(buffer,MetaElements_,ts);//gets the 'length' element.


		Tag tag=MetaElements_.begin()->first;
		if(tag!=TAG_FILE_INFO_GR_LEN)
			throw exception("First tag must be group length(0x0002,0000) in File Meta Information");
		Value& value=MetaElements_.begin()->second;

		UINT32 FileMetaInfoLength;
		value >> FileMetaInfoLength;

		//now read the rest of the file meta info from the input stream

		Buffer buffer2(__LITTLE_ENDIAN);

		for(UINT32 i=0;i<FileMetaInfoLength;i++)
			buffer2.push_back(In.get());

		//now parse onto meta info set.
		ReadFromBuffer(buffer2,MetaElements_,ts);

		//done, and I should be at the correct point in the file to continue reading!
	}


	int FileMetaInformation::Write(std::ostream& Out)
	{
		Out.seekp(0);//always go to the beginning

		//first the preamble...
		for(int i=0;i<128;i++)
			//Out.put(0);
			Out.put(Preamble_[i]);   
		

		Out.put('D');
		Out.put('I');
		Out.put('C');
		Out.put('M');

		Buffer buffer(__LITTLE_ENDIAN);


		WriteToBuffer(MetaElements_,buffer,TS(EXPL_VR_LE_TRANSFER_SYNTAX/*TS::EXPL_VR_LE*/));//Section 7.1 says this syntax has to be used.

		while(buffer.position()!=buffer.end())
		{
			Out.put(*buffer.position());
			buffer.increment(1);
		}

		return (buffer.size() + 128 + 4);

	}

}//namespace dicom
