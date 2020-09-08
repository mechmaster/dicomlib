#include <algorithm>
#include <cctype>

#include "Utility.hpp"

void StripTrailingWhitespace(std::string& str)
{
  str.erase(remove_if(str.begin(), str.end(), isspace), str.end());

  /*
  Technically speaking, we shouldn't have to do this, but we have
  encountered some GE images that have NULLS at the end of strings
  rather than whitespace.

  */
  StripTrailingNull(str);
}


void StripTrailingNull(std::string& str)
{
  str.erase(remove_if(str.begin(), str.end(),
    [](std::uint8_t ch) -> bool
    {
      return ch == '\0';
    }
    ), str.end());
}

bool IsDigitString(std::string& str)
{
  return std::all_of(str.begin(), str.end(), isdigit);
}

/*
Occasionaly the standard calls for a number unique to a given
association (see e.g. Part 8, Section 7.1.1.13)

The implementation here is insufficiently robust - the above variable
'uniqid' should be a member of the object responsible for negotiating
the association.
*/

std::uint16_t getMessageID()
{
  static std::uint16_t messageID = 1;
  ++messageID;

  return messageID;
}
