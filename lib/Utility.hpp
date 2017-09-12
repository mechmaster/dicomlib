#pragma once

#include <string>

#include "Types.hpp"

//these should be in dicom namespace

//!Removes leading and trailing whitespace from a string.
void StripTrailingWhitespace(std::string& str);

void StripTrailingNull(std::string& str);

bool IsDigitString(std::string& str);

//The following are GENERATORS//

template <typename T>
struct Series
{
  T m_currentValue;

  Series() :
    m_currentValue(0)
  {
  }

  T operator()()
  {
    return m_currentValue++;
  }
};

template <typename T>
struct OddSeries
{
  T m_currentValue;

  OddSeries() :
    m_currentValue(1)
  {
  }

  T operator()()
  {
    T ret = m_currentValue;
    m_currentValue += 2;
    return ret;
  }
};

std::uint16_t getMessageID();
