/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "Dumper.hpp"
#include "DataDictionary.hpp"
#include "ValueToStream.hpp"
using std::ostream;
namespace dicom
{
	struct Dumper
	{
		std::ostream& Out_;
		Dumper(std::ostream& Out);
		void operator()(const DataSet::value_type& v);
	};

	Dumper::Dumper(ostream& Out):Out_(Out)
	{
	}
	void Dumper::operator()(const DataSet::value_type& v)
	{
		Out_ << GetName(v.first) << "\t" << v.second << std::endl;
	}

	void Dump(const DataSet& data,std::ostream& Out)
	{
		std::for_each(data.begin(),data.end(),Dumper(Out));
	}


	ostream& operator <<  (ostream& Out, const DataSet& data)
	{
		Dump(data,Out);
		return Out;
	}
}//namespace dicom
