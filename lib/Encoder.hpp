#pragma once

#include <exception>

#include "DataSet.hpp"
#include "TransferSyntax.hpp"
#include "Buffer.hpp"

/*
  As the following class basically only performs one job,
  maybe it should expose itself as a simple function call?
  See comments in Decoder.hpp
*/

namespace dicom
{
  struct EncoderError : public std::exception{};

  UINT32 WriteToBuffer(const DataSet& data, Buffer& buffer, TS transfer_syntax);

  class Encoder
  {
    Buffer& m_buffer;
    const DataSet& m_dataset;
    TS m_ts;

  public:

    Encoder(Buffer& buffer, const DataSet& ds, TS ts);
    UINT32 encode();

  private:
    
    UINT32 encodeElement(const DataSet::value_type& element) const;
    UINT32 writeLengthAndVR(UINT32 length, VR vr);
    UINT32 sendRange(DataSet::const_iterator Begin, DataSet::const_iterator End);
    UINT32 sendSequenceItemInExplicitLength(Buffer& B, const DataSet& SqItem);
    UINT32 sendSequence(const Sequence& sequence, bool explicit_length = true);
    UINT32 sendAttributeTag(DataSet::const_iterator Begin, DataSet::const_iterator End);
    UINT32 sendUID(DataSet::const_iterator Begin, DataSet::const_iterator End);
    UINT32 sendOB(DataSet::const_iterator Begin, DataSet::const_iterator End);

    template<VR vr>
    UINT32 sendFundamentalType(DataSet::const_iterator Begin, DataSet::const_iterator End)
    {
      UINT32 sentlength = 0;

      typedef typename TypeFromVR<vr>::Type Type;
      BOOST_STATIC_ASSERT(boost::is_fundamental<Type>::value);

      std::vector<Value> vv;
      for (; Begin != End; Begin++)
      {
        if (Begin->second.empty())
        {
          continue;
        }

        vv.push_back(Begin->second);
      }
      
      UINT32 Length = (UINT32)vv.size();//Should be identical to End-Begin, but we don't have subtraction operator available.
      sentlength += writeLengthAndVR(sizeof(Type) * Length, vr);

      for (size_t i = 0; i < vv.size(); i++)
      {
        Type data;
        vv[i] >> data;
        m_buffer << data;
        sentlength += sizeof(Type);
      }

      return sentlength;
    }

    template <VR vr>
    UINT32 sendString(DataSet::const_iterator Begin, DataSet::const_iterator End)
    {
      UINT32 sentlength = 0;

      StaticVRCheck<std::string, vr>();

      Tag tag = Begin->first;

      //delimiter character is '\'
      std::string StringToSend;
      for(; Begin != End; Begin++)
      {
        std::string s;
        const Value& value = Begin->second;

        if((vr != value.vr()) || (tag != Begin->first)) // this block is basically an ASSERT, ie I expect it to never be entered.
        {
          //we have a major problem.
          throw dicom::exception("Some inconsistency in dataset.");
        }

        value >> s;

        StringToSend.append(s);
        StringToSend.append(1, '\\'); //delimiter
      }

      StringToSend.erase(StringToSend.end() - 1); //remove last delimiter.

      if(StringToSend.size() & 0x01)//length is odd
      {
        StringToSend.append(1, ' ');//string length must be even.
      }
        
      sentlength += writeLengthAndVR((UINT32)StringToSend.size(), vr);
      m_buffer << StringToSend;
      sentlength += StringToSend.length();

      return sentlength;
    }
  };
}//namespace dicom

