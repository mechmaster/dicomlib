#include <iostream>
#include <assert.h>

#include "Decoder.hpp"
#include "iso646.h"

#include "pdata.hpp"

/*
This is seriously ugly, and quite misleading.  We definitely need some clean up work here!
*/

namespace dicom
{

/*
I think the object model presented in this file
is sub-optimal.
*/

/*!
defined in Part8, table 9-23
*/
class PDV//presentation data value item
{

public:

  std::uint32_t m_length;
  std::uint8_t m_presentationContextID;
  std::uint8_t m_messageHeader;

//defined in Part 8, Annex E.2, see figure E.2-1

  bool IsCmdMessage()
  {
    return (m_messageHeader & MessageControlHeader::COMMAND);
  }

  bool IsDataMessage()
  {
    return !(m_messageHeader & MessageControlHeader::COMMAND);
  }

  bool IsLastFragment()
  {
    return ((m_messageHeader & MessageControlHeader::LAST_FRAGMENT) != 0);
  }
};

/*!
documented in Part 8, section 7.6,  figure  9-2 and tables 9-22 and 9-23
*/

bool PDataTF::read(Buffer& temp)
{
  std::uint32_t count;

  if(!m_length)//why would this ever not be the case?
  {
    std::uint8_t reservedDummy;
    temp >> reservedDummy;//shouldn't this fail?
    temp >> m_length;
  }

  count = m_length;
  m_msgStatus = 0; // continue
  PDV pdv;//should be inside following loop, i think

  while (count > 0)
  {
    //I think that this should happen in a member of PDV-.
    temp >> pdv.m_length;
    temp >> pdv.m_presentationContextID;
    temp >> pdv.m_messageHeader;

    //now read actual data from socket onto buffer.

    temp >> m_buffer;

    /*
    The previous 3 lines could be dramatically speeded up if
    we _first_ allocate the space on buffer, then pass _pointers_
    to the begin and end of the new space to Socket::Readn();

    I'm not going to change this till I have a good unit test for the change

    Meyers says in 'Efficient STL' that member functions are preferable
    over algorithms for efficiency?

    So possible optimizations would be:
    1)

    buffer_.insert(buffer_.end(),data.begin(),data.end());//this is simple.

    2)

    buffer_.insert(buffer_.end(),pdv.Length-2,0x00);
    socket.Readn(buffer_.end()-(pdv.Length-2),(pdv.Length-2));//slightly more complicated.

    We need profiling tests for both, such as dicomtest::SubmitLotsOfImages()
    */

    count = count - pdv.m_length - sizeof(std::uint32_t);
    m_length = m_length - pdv.m_length - sizeof(std::uint32_t);

    if (pdv.IsLastFragment())
    {
      m_msgStatus = 1;
      m_presentationContextID = pdv.m_presentationContextID;
      return true;
    }
  }

  if(pdv.IsLastFragment())
  {
    assert(0);//how can this ever happen?
    m_msgStatus = 1;
  }

  m_presentationContextID = pdv.m_presentationContextID;

  return true;
}

PDataTF::PDataTF(int ByteOrder) :
  m_buffer(ByteOrder),
  m_length(0)
{
}
}//namespace dicom
