/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "File.hpp"
#include "Buffer.hpp"
#include "TransferSyntax.hpp"
#include "Decoder.hpp"
#include "Encoder.hpp"


namespace dicom
{

/*
	TODO
	Are there more efficient ways of reading/writing bytes to a stream
	than the one I'm using here?  I'm sure there must be...
*/

	/*!
		Is there a nicer way of implementing this?

	*/
	size_t GetStreamSize(std::ifstream& In)
	{
		std::streampos CurrentPosition=In.tellg();	//get current position.
		In.seekg(0,std::ios::end);					//jump to end
		long StreamSize=In.tellg();					//get stream length.
		In.seekg(CurrentPosition);					//reset
		return StreamSize;
	}

	void ReadFileMetaFromStream(std::ifstream& In, DataSet& ds)
	{//added by Sam Shen - I don't see why meta info is not part of the data set
		ds.clear();
		In.seekg(0);
		UID TransferSyntaxUID=IMPL_VR_LE_TRANSFER_SYNTAX;//default
		try
		{
			FileMetaInformation MetaInfo(In);
			MetaInfo.MetaElements_(TAG_TRANSFER_SYNTAX_UID) >> TransferSyntaxUID;
			ds = MetaInfo.MetaElements_;
		}
		catch (FileMetaInfoException& e)
		{

			/*
				If FileMetaInformation constructor fails, it probably
				means we're opening a KODAK file that doesn't have meta info.
				(this is a violation of the DICOM standard.)
				Against my better judgement I've been asked to make
				the code try and recover at this point.  I can't bring
				myself to, but if you really need it, remove the following
				re-throw command
			*/
			throw;
			In.seekg(0);
		}
		return ;
	}

	/*
	I add a limit to the buffer so that I can read the header only -Sam 29July2009
	*/
	void ReadFromStream(std::ifstream& In, DataSet& data,size_t max_number_of_byte_to_read)
	{
		data.clear();


		UID TransferSyntaxUID=IMPL_VR_LE_TRANSFER_SYNTAX;//default
		


		try
		{
			FileMetaInformation MetaInfo(In);
			MetaInfo.MetaElements_(TAG_TRANSFER_SYNTAX_UID) >> TransferSyntaxUID;
		}
		catch (FileMetaInfoException& e)
		{

			/*
				If FileMetaInformation constructor fails, it probably
				means we're opening a KODAK file that doesn't have meta info.
				(this is a violation of the DICOM standard.)
				Against my better judgement I've been asked to make
				the code try and recover at this point.  I can't bring
				myself to, but if you really need it, remove the following
				re-throw command
			*/
			throw;
			In.seekg(0);
		}

		
		TS ts(TransferSyntaxUID);
		/*
			if MetaInformation constructs correctly, then
			In should be pointing at the correct point in the
			file to start reading the DataSet.
		*/

		size_t BytesToRead=GetStreamSize(In)-In.tellg();
		BytesToRead=std::min(BytesToRead,max_number_of_byte_to_read);

		int ByteOrder=ts.isBigEndian()?
			__BIG_ENDIAN:__LITTLE_ENDIAN;

		Buffer buffer(ByteOrder);

		//Read data from stream onto buffer.

		buffer.assign(BytesToRead,0);
		unsigned char* pData=&buffer.front();;

		In.read((char*)pData,BytesToRead);//This is the most time intensive part. Can we speed it up any?
		//Transfer data from buffer onto dataset

		ReadFromBuffer(buffer,data,ts);

		//I insist on putting the ts into the data so that user has an chance to interpret pixel data correctly. -Sam
		if(ts.isEncapsulated() || ts.isDeflated())
			data.Put<VR_UI>(TAG_TRANSFER_SYNTAX_UID,TransferSyntaxUID);
	}

	void WriteToStream(const DataSet& data,std::ostream& Out,TS ts, bool Tiff)
	{
		FileMetaInformation MetaInfo(data,ts);
		int bytes_in_meta=MetaInfo.Write(Out); //We write it now to get the buffer size of MeatInfo 
									           //rewrite when we get enough infomation to put to TIFF header

		int ByteOrder=ts.isBigEndian()?
			__BIG_ENDIAN:__LITTLE_ENDIAN;

		Buffer buffer(ByteOrder);
		UID TS_UID=MetaInfo.MetaElements_(TAG_TRANSFER_SYNTAX_UID).Get<UID>();

		WriteToBuffer(data,buffer,TS(TS_UID));


		/*
			The following is in desperate need of optimisation!
			UCDMC99 had a 'WriteToStream' version of WriteToBuffer,
			that matched exactly the syntax of WriteToBuffer.  Two identical
			functions is bad, can we trick it with templates or
			virtual functions??
		*/

		/*
		while(buffer.position()!=buffer.end())
		{
			Out.put(*buffer.position());
			buffer.Increment(1);
		}*/

		int bytes_to_write=buffer.end()-buffer.position();

		if(Tiff)
		{
			FileMetaInformation MetaInfoTiff(data,ts,bytes_to_write + bytes_in_meta);
			MetaInfoTiff.Write(Out);
		}

		Out.write((const char *)(&*buffer.position()),bytes_to_write);
		buffer.Increment(bytes_to_write);
	}

	/*
	I add a limit to the buffer so that I can read the header only -Sam 29July2009
	*/
	void Read(std::string FileName,DataSet& data,size_t max_number_of_byte_to_read)
	{
		std::ifstream in(FileName.c_str(),std::ios::binary);
        
        if(in.fail())
            throw dicom::exception("Couldn't open input file");
            

		ReadFromStream(in,data,max_number_of_byte_to_read);
	}

	//supply a false if you want to write a Pure Dicom file
	//here pure means NOT-TIFF-Compatible
	void Write(const DataSet& data, std::string FileName, TS ts, bool Tiff)
	{
		std::ofstream out(FileName.c_str(),std::ios::binary);//what's default behaviour if file already exists?
		WriteToStream(data,out,ts, Tiff);
	}

}//namespace dicom
