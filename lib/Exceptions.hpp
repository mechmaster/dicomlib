#ifndef EXCEPTIONS_HPP_INCLUDE_GUARD_57385720348
#define EXCEPTIONS_HPP_INCLUDE_GUARD_57385720348
#if defined(_MSC_VER)//windows compiler
	#pragma warning(disable:4290)  // Disable warning about throw specifications
#endif

/*
	Exception classes specific to dicom library.
	
	TODO:
		Would like a way of logging exceptions.
*/

#include <exception>
#include <string>
namespace dicom
{


	//!dicom::Exception is the root exception class
    /*!
        Shouldn't this inherit from std::runtime_error?
    */
	class exception : public std::exception
	{
	protected:

		 std::string What_;
	public:
	
		//!no reference, just a description
		exception(std::string What);

		//!g++ needs this
		virtual ~exception()  throw(){}
		
		virtual const char* what() const throw();
		
	};
	
	
	/*!
		Throw if the code SHOULD work, but doesn't, because the functionality
		hasn't been implemented yet.  This class should be removed before
		the library reaches release status - any instance of this class
		is an indicator of incompleteness.
	*/
	struct NotYetImplemented: public dicom::exception
	{
		NotYetImplemented():exception("Feature not yet implemented."){}
		//!To keep gcc happy
		virtual ~NotYetImplemented()  throw(){}
	};


	
	//!thrown if a Association Negotiation failed.
	struct FailedAssociation : public dicom::exception
	{
		FailedAssociation():exception("Failed Association"){}
		virtual ~FailedAssociation() throw(){}
	};

	//!throws a dicom::exception if Condition is not true
	void Enforce(bool Condition,std::string Comment="Failed Enforcement");


}//namespace dicom
#endif //EXCEPTIONS_HPP_INCLUDE_GUARD_57385720348
