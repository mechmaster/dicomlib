#pragma once

#if defined(_MSC_VER) //windows compiler
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

    std::string m_what;

  public:

    //!no reference, just a description
    exception(std::string what);

    //!g++ needs this
    virtual ~exception() throw(){}
    virtual const char* what() const throw();
  };

  /*!
  Throw if the code SHOULD work, but doesn't, because the functionality
  hasn't been implemented yet.  This class should be removed before
  the library reaches release status - any instance of this class
  is an indicator of incompleteness.
  */

  class NotYetImplemented: public dicom::exception
  {
  public:

    NotYetImplemented() :
      exception("Feature not yet implemented.")
    {
    }

    //!To keep gcc happy
    virtual ~NotYetImplemented() throw()
    {
    }
  };

  //!thrown if a Association Negotiation failed.
  class FailedAssociation : public dicom::exception
  {
  public:

    FailedAssociation() :
      exception("Failed Association")
    {
    }

    virtual ~FailedAssociation() throw()
    {
    }
  };

  //!throws a dicom::exception if Condition is not true
  void Enforce(bool Condition, std::string Comment = "Failed Enforcement");
}//namespace dicom

