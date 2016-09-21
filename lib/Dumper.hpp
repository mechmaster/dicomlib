
#ifndef DUMPER_HPP_INCLUDE_GUARD_123416013
#define DUMPER_HPP_INCLUDE_GUARD_123416013

#include <ostream>
#include "DataSet.hpp"
namespace dicom
{

	//!Prints the contents of 'data' to the provided output stream.
	void Dump(const DataSet& data,std::ostream& Out=std::cout);

	//!operator does the same as Dump()
	std::ostream& operator <<  (std::ostream& out, const dicom::DataSet& data);
}

#endif //DUMPER_HPP_INCLUDE_GUARD_123416013
