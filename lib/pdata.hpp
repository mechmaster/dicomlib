/****************************************************************************
 *
 * Abstract Base Class for P-DATA Service
 *
 *
 ****************************************************************************/

#pragma once

#include "Types.hpp"
#include "DataSet.hpp"
#include "Buffer.hpp"

/*
We could redesign the following two classes to mirror the standard better
by having PDataTF HAVE A list<> of PDVs.  I think we can avoid copying-related
performance issues if this is THE place where the associated DataSets reside, and
they are passed to other classes by reference.  After all, I don't see why a
Command or Data message would ever be changed after it had been received.
*/

namespace dicom
{
  namespace MessageControlHeader
  {
    typedef std::uint8_t Code;
    const Code DATASET = 0x00;
    const Code COMMAND = 0x01;
    const Code LAST_FRAGMENT = 0x02;
  }

/*!
defined in Part8, table 9-22
Further documentation in Part 8, Annex E

I think this class needs to be seriously re-thought, or even removed altogether,
as it's implementation is very misleading.  This class does _not_ corresponding
exactly to the P_DATA_TF request primitive, rather it serves to manage a _stream_
of them!

We need to know the maximum length specified during association negotiation.

*/

  class PDataTF
  {
    static const std::uint8_t m_itemType = 0x04;
    static const std::uint8_t m_reserved1 = 0x00;

  public:

    Buffer m_buffer;

    std::uint32_t m_length;

    std::int32_t m_msgStatus;
    std::uint8_t m_presentationContextID;
    std::uint8_t m_messageHeader;

    PDataTF(int ByteOrder);
    
    bool read(Buffer& temp);
  };
}//namespace dicom

