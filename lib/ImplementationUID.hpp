#ifndef IMPLEMENTATION_UID_HPP_INCLUDE_GUARD_358394857
#define IMPLEMENTATION_UID_HPP_INCLUDE_GUARD_358394857
#include <string>


/*
	TODO
	Work still needs to be done here.

	See Part 7, Annex D.3.3.2 for
	how the following values SHOULD be used and defined.
	
	Please fix.

*/
namespace dicom
{
	//const std::string ImplementationClassUID = "1.2.840.416.480.5705.1234";//pretty much random, no policy implemented.
	const std::string ImplementationClassUID = "1.2.826.0.1.3680043.2.1553";//UID prefix supplied by Medical Connection in Sep 2006
	const std::string ImplementationVersionName = "DICOMLIB2008";
}//namespace dicom



#endif //IMPLEMENTATION_UID_HPP_INCLUDE_GUARD_358394857
