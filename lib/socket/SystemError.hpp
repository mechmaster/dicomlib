#ifndef SYSTEM_ERROR_INCLUDE_GUARD_573938574
#define SYSTEM_ERROR_INCLUDE_GUARD_573938574

#include <exception>
#include <string>
#include <sstream>

//!Thrown after a failed system call.
/*!
This class is designed to be thrown directly
after a failed system call.  It makes a local copy of
errno, which then gets translated to a human-readable
errorstring when what() is called.

sample usage:

try
{
int s=socket(...);
if(s<=0)
throw SystemError("Socket call failed.");
}
catch(std::exception& e)
{
std::cout << e.what() << std::endl;
}

Note that it is important to throw this DIRECTLY after the failed
system call, as subsequent calls may change the value of errno.
A second constructor is supplied that allows the user to specify
the error number.
*/

class SystemError : public std::exception
{
	//!store a local copy of errno, as any system call can change it.
	int Error_;
	//!user specified description.
	std::string Description_;

	std::string What_;//used to manage char* returned from what()
public:

	SystemError(std::string Description):Description_(Description),Error_(errno)
	{
		std::stringstream s;
		s<< Description_.c_str() << ", " << strerror(Error_) << "." << std::endl;
		What_ = s.str();
	}


	SystemError(std::string Description,int ErrorNumber):Description_(Description),Error_(ErrorNumber)
	{


		std::stringstream s;
		s<< Description_.c_str() << ", " << strerror(Error_) << "." << std::endl;
		What_ = s.str();
	}

	const char* what() const throw()
	{
		return What_.c_str();
	}

	//!Returns users description and system error description (returned from strerror)

	virtual ~SystemError() throw(){}

	int Number()const{return Error_;}
};

#endif//SYSTEM_ERROR_INCLUDE_GUARD_573938574
