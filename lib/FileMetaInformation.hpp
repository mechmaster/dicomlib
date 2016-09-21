#ifndef FILE_META_INFORMATION_HPP_INCLUDE_GUARD_784364843843
#define FILE_META_INFORMATION_HPP_INCLUDE_GUARD_784364843843
#include <iostream>
#include "DataSet.hpp"
#include "Exceptions.hpp"
#include "TransferSyntax.hpp"
#include "ImplementationUID.hpp"


namespace dicom
{

	struct FileMetaInfoException:public dicom::exception
	{
		FileMetaInfoException(std::string Description):exception(Description){}
	};

	//!DICOM File Meta Information
	/*!
		Documented in Part 10, Section 7.1

		Consists of:
		Preamble (128 bytes)
		DICOM prefix (4 bytes)
		File Meta Elements (Part 10, table 7.1-1)
	*/
	class FileMetaInformation
	{
	public:
		/*
			What constructors are we going to allow?
			-Read _from_ a stream
			-construct with the intention of writing _to_ a stream.
		*/
		FileMetaInformation(std::istream& In);
		//FileMetaInformation(const UID& classUID, const UID& instUID, TS ts);

		//!Infer instance and class UIDs from data set
		FileMetaInformation(const DataSet& data,TS ts);
		
		//to make a Tiff-Dicom !mge @May 2009
		FileMetaInformation(const DataSet& data,TS ts,long BytesToWrite);

		char Preamble_[128];
		DataSet MetaElements_;
		int/*void*/ Write(std::ostream& Out);

	};


}//namespace dicom


#endif //FILE_META_INFORMATION_HPP_INCLUDE_GUARD_784364843843
