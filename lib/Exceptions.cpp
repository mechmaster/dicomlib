/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

//Implementation of exception classes.

#include "Exceptions.hpp"

namespace dicom
{
  exception::exception(std::string what) :
    m_what(what)
  {
  }

  const char* exception::what() const throw()
  {
    return m_what.c_str();
  }

  /*!
  Simplified from http://www.cuj.com/documents/s=8250/cujcexp2106alexandr/alexandr.htm
  */
  void Enforce(bool Condition, std::string Comment)
  {
    if(!Condition)
    {
      throw dicom::exception(Comment);
    }
  }
}
