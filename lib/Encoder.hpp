#pragma once

#include <exception>

#include "Buffer.hpp"
#include "DataSet.hpp"
#include "TransferSyntax.hpp"

/*
  As the following class basically only performs one job,
  maybe it should expose itself as a simple function call?
  See comments in Decoder.hpp
*/

namespace dicom
{
  std::uint32_t WriteToBuffer(const DataSet& data, Buffer& buffer, TS transfer_syntax);

  class Encoder
  {
    Buffer& m_buffer;
    const DataSet& m_dataset;
    TS m_ts;

  public:

    Encoder(Buffer& buffer, const DataSet& ds, TS ts);
    std::uint32_t encode();

  private:
    
    std::uint32_t encodeElement(const DataSet::value_type& element) const;
    std::uint32_t writeLengthAndVR(std::uint32_t length, VR vr);
    std::uint32_t sendRange(DataSet::const_iterator Begin, DataSet::const_iterator End);
    std::uint32_t sendSequenceItemInExplicitLength(Buffer& B, const DataSet& SqItem);
    std::uint32_t sendSequence(const Sequence& sequence, bool explicit_length = true);
    std::uint32_t sendAttributeTag(DataSet::const_iterator Begin, DataSet::const_iterator End);
    std::uint32_t sendUID(DataSet::const_iterator Begin, DataSet::const_iterator End);
    std::uint32_t sendOB(DataSet::const_iterator Begin, DataSet::const_iterator End);

    template<VR vr>
    std::uint32_t sendFundamentalType(DataSet::const_iterator Begin, DataSet::const_iterator End)
    {
      std::uint32_t sentlength = 0;

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
      
      std::uint32_t Length = (std::uint32_t)vv.size();//Should be identical to End-Begin, but we don't have subtraction operator available.
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
    std::uint32_t sendString(DataSet::const_iterator Begin, DataSet::const_iterator End)
    {
      std::uint32_t sentlength = 0;

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
        
      sentlength += writeLengthAndVR((std::uint32_t)StringToSend.size(), vr);
      m_buffer << StringToSend;
      sentlength += StringToSend.length();

      return sentlength;
    }
  };
}//namespace dicom

