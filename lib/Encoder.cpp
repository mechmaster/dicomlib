/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <iostream>

#include "Encoder.hpp"

namespace dicom
{
  Encoder::Encoder(Buffer& buffer, const DataSet& ds, TS ts) :
    m_buffer(buffer),
    m_dataset(ds),
    m_ts(ts)
  {
  }
  
  UINT32 Encoder::sendAttributeTag(DataSet::const_iterator Begin, DataSet::const_iterator End)
  {
    UINT32 sentlength = 0;
    UINT32 Length = (UINT32)m_dataset.count(Begin->first);//Should be identical to End-Begin, but we don't have subtraction operator available.
    
    sentlength += writeLengthAndVR(sizeof(Tag) * Length, VR_AT);
    
    for (; Begin != End; Begin++)
    {
      Tag data;
      Begin->second >> data;
      m_buffer << data;
      sentlength += sizeof(Tag);
    }
    
    return sentlength;
  }
  
  UINT32 Encoder::sendUID(DataSet::const_iterator Begin, DataSet::const_iterator End)
  {
    UINT32 sentlength = 0;
    
    Tag tag=Begin->first;
    
    std::string StringToSend;
    for(; Begin != End; Begin++)
    {
      UID uid;
      const Value& value = Begin->second;
      
      if((VR_UI != value.vr()) or (tag != Begin->first))
      {
        throw dicom::exception("Some inconsistency in dataset.");
      }
      
      value >> uid;
      StringToSend.append(uid.str());
      StringToSend.append(1, '\\');//delimiter
    }
    
    StringToSend.erase(StringToSend.end() - 1);//remove last delimiter.
    
    if(StringToSend.size() bitand 0x01) //length is odd
    {
      StringToSend.append(1, '\0'); //length must be even, NULL character is used for padding UIDs
    }
    
    sentlength += writeLengthAndVR((UINT32)StringToSend.size(), VR_UI);
    m_buffer << StringToSend;
    sentlength += StringToSend.length();
    
    return sentlength;
  }
  
  UINT32 Encoder::sendOB(DataSet::const_iterator Begin, DataSet::const_iterator End)
  {
    UINT32 sentlength = 0;
    typedef TypeFromVR<VR_OB>::Type Type;
    
    Tag tag = Begin->first;
    
    int fragments = m_dataset.count(tag);
    
    Enforce(m_ts.isEncapsulated() || (1 == fragments), "Only encoded data can have multiple image fragments.");
    
    if(1 == fragments)//just send the data
    {
      const Type& ByteVector = Begin->second.Get<Type>();
      sentlength += writeLengthAndVR((UINT32)ByteVector.size(), VR_OB);
      m_buffer.AddVector(ByteVector);
      sentlength += ByteVector.size();
    }
    else //send the data as a series of fragments as defined in Part5 Annex 4
    {
      sentlength += writeLengthAndVR(UNDEFINED_LENGTH, VR_OB);
      
      //send empty offset table - not supported.
      
      m_buffer << TAG_ITEM;
      sentlength += sizeof(Tag);
      m_buffer << UINT32(0x00); //no offset table.
      sentlength += sizeof(UINT32);
      
      for(;Begin != End; Begin++)
      {
        m_buffer << TAG_ITEM;
        sentlength += sizeof(Tag);
        
        const Type& ByteVector = Begin->second.Get<Type>();
        m_buffer << UINT32(ByteVector.size());
        sentlength += sizeof(UINT32);
        
        m_buffer.AddVector(ByteVector);
        sentlength += ByteVector.size();
      }
      
      m_buffer << TAG_SEQ_DELIM_ITEM;
      sentlength += sizeof(Tag);
      m_buffer << UINT32(0x00);
      sentlength += sizeof(UINT32);
    }
    
    return sentlength;
  }
  
  UINT32 Encoder::sendRange(DataSet::const_iterator Begin, DataSet::const_iterator End)
  {
    //we might want an ASSERT here to check that the range truly is consistent,
    //i.e. only consists of elements sharing the same Tag and VR...
    UINT32 sentlength = 0;
    Tag tag = Begin->first;
    VR vr = Begin->second.vr();

    m_buffer << tag;
    sentlength += sizeof(Tag);

    switch (vr)
    {
      case VR_AE:
        sentlength += sendString<VR_AE>(Begin, End);
      break;
      case VR_AS:
        sentlength += sendString<VR_AS>(Begin, End);
      break;
      case VR_AT:
        sentlength += sendAttributeTag(Begin, End);
      break;
      case VR_CS:
        sentlength += sendString<VR_CS>(Begin, End);
      break;
      case VR_DA:
        sentlength += sendString<VR_DA>(Begin, End);
      break;
      case VR_DS:
        sentlength += sendString<VR_DS>(Begin, End);
      break;
      case VR_DT:
        sentlength += sendString<VR_DT>(Begin, End);
      break;
      case VR_FD:
        sentlength += sendFundamentalType<VR_FD>(Begin, End);
      break;
      case VR_FL:
        sentlength += sendFundamentalType<VR_FL>(Begin, End);
      break;
      case VR_IS:
        sentlength += sendString<VR_IS>(Begin, End);
      break;
      case VR_LO:
        sentlength += sendString<VR_LO>(Begin, End);
      break;
      case VR_LT:
        sentlength += sendString<VR_LT>(Begin, End);
      break;
      case VR_OB:
        sentlength += sendOB(Begin, End);
      break;
      case VR_OW:
        {
          typedef TypeFromVR<VR_OW>::Type Type;
          const Type& WordVector = Begin->second.Get<Type>();

          UINT32 ByteLength = WordVector.size() * 2;
          sentlength += writeLengthAndVR(ByteLength, VR_OW);

          m_buffer.AddVector(WordVector);
          sentlength += ByteLength;
        }
      break;
      case VR_PN:
        sentlength += sendString<VR_PN>(Begin, End);
      break;
      case VR_SH:
        sentlength += sendString<VR_SH>(Begin, End);
      break;
      case VR_SL:
        sentlength += sendFundamentalType<VR_SL>(Begin, End);
      break;
      case VR_SQ:
        {
          const Sequence& sequence = Begin->second.Get<Sequence>();
          sentlength += sendSequence(sequence);
        }
      break;
      case VR_SS:
        sentlength += sendFundamentalType<VR_SS>(Begin, End);
      break;
      case VR_ST:
        sentlength += sendString<VR_ST>(Begin, End);
      break;
      case VR_TM:
        sentlength += sendString<VR_TM>(Begin, End);
      break;
      case VR_UI:
        sentlength += sendUID(Begin, End);
      break;
      case VR_UL:
        sentlength += sendFundamentalType<VR_UL>(Begin, End);
      break;
      case VR_UN:
        {
          typedef TypeFromVR<VR_UN>::Type Type;
          const Type& ByteVector = Begin->second.Get<Type>();
          sentlength += writeLengthAndVR((UINT32)ByteVector.size(), VR_UN);
          m_buffer.AddVector(ByteVector);
          sentlength += ByteVector.size();
        }
      break;
      case VR_US:
        sentlength += sendFundamentalType<VR_US>(Begin, End);
      break;
      case VR_UT:
        sentlength += sendString<VR_UT>(Begin, End);
      break;
      default:
        std::cout << "Unknown VR: " << vr  << " in EncodeElement()" << std::endl;
        throw BadVR(vr);
    }

    return sentlength;
  }

  UINT32 Encoder::writeLengthAndVR(UINT32 length, VR vr)
  {
    UINT32 sentlength = 0;

    if (m_ts.isExplicitVR())
    {
      m_buffer << BYTE(vr); //byte 1 -Sam
      m_buffer << BYTE(vr >> 8); //byte 2 -Sam
      sentlength += 2;

      if (VR_UN == vr || VR_SQ == vr || VR_OW == vr || VR_OB == vr || VR_UT == vr)
      {
        m_buffer << UINT16(0); //reserved
        sentlength += sizeof(UINT16);
        m_buffer << length; //4 bytes
        sentlength += sizeof(UINT32);
      }
      else
      {
        m_buffer << UINT16(length); //2 bytes
        sentlength += sizeof(UINT16);
      }
    }
    else
    {
      //no VR info sent
      m_buffer << length; //4 bytes
      sentlength += sizeof(UINT32);
    }

    return sentlength;
  }

  UINT32 Encoder::encode()
  {
    UINT32 sentlength = 0;
    DataSet::const_iterator I = m_dataset.begin();

    while(I != m_dataset.end())
    {
      Tag tag = I->first;
      std::pair<DataSet::const_iterator, DataSet::const_iterator> p = m_dataset.equal_range(tag);
      sentlength += sendRange(p.first, p.second);
      I = p.second;
    }

    return sentlength;
  }

  /*!

  This should be simpler than Decoder::DecodeSequence(), because we only need
  to implement ONE way of sending multiple data sets.
  Which shall we use?  Exlicit length, or Sequence Delimitation Items?  I
  think the second is the simplest... -Trevor

  When I deal with programs using OFFIS dcmtk (OFFIS_DCMTK_354), I have a problem 
  that a program with OFFIS can not interpret the undefined length correctly. I try 
  to implement explicit length like tables in 7.5-1,2,3 in DICOM Part 5. I have
  to add one more level of send, the SequenceItem. One difficulty of implementing
  explicit length is that we do not know the length of the sequence until we finish
  sending it, but DICOM like us to send the length before the items. I will have to 
  send the sequence item to another buffer and calculate its length. Then copy the
  new buffer to the original buffer. -Sam Shen
  */

  UINT32 Encoder::sendSequenceItemInExplicitLength(Buffer& B, const DataSet& SqItem)
  {
    UINT32 sentlength = 0;
    int ByteOrder = m_ts.isBigEndian() ? __BIG_ENDIAN : __LITTLE_ENDIAN;
    Buffer tmp(ByteOrder);

    Encoder E(tmp, SqItem, m_ts);//Encoder to a tmp buffer -Sam
    sentlength += E.encode();//coding the sequence into tmp and get the length -Sam

    B << TAG_ITEM; //Now, I can load the sq item into B, knowing the length -Sam
    B << sentlength;
    sentlength += 8;
    B.insert(B.end(), tmp.begin(), tmp.end());

    return sentlength;
  }

  UINT32 Encoder::sendSequence(const Sequence& sequence, bool explicit_length)
  {
    if (sequence.size() == 0)
    {
      return writeLengthAndVR(UINT32(0), VR_SQ);
    }

    UINT32 sentlength = 0;
    if (!explicit_length)
    {
      writeLengthAndVR(UNDEFINED_LENGTH, VR_SQ);

      //Encoding the sequence item of undefined length sequence. In Part 5 Table 7.5-3,
      //undfined length sequence can contain explicit length sequence item. However, I 
      //will only implement undefined length item here for simpicity. -Sam Shen
      for(Sequence::const_iterator I = sequence.begin(); I != sequence.end(); I++)
      {
        m_buffer << TAG_ITEM;
        m_buffer << UNDEFINED_LENGTH;

        Encoder E(m_buffer, *I, m_ts);
        E.encode();

        m_buffer << TAG_ITEM_DELIM_ITEM;
        m_buffer << UINT32(0x00);
      }

      m_buffer << TAG_SEQ_DELIM_ITEM;
      m_buffer << UINT32(0x00);
      return UNDEFINED_LENGTH;
    }
    else//explicit length
    {
      //Create a new buffer
      //send sequence to this new buffer util done and record the length of new buffer
      //send the vr and length to original buffer
      //append the new buffer to original buffer

      int ByteOrder = m_ts.isBigEndian() ? __BIG_ENDIAN : __LITTLE_ENDIAN;

      Buffer B(ByteOrder);

      for(Sequence::const_iterator I = sequence.begin(); I != sequence.end(); I++)
      {
        sentlength += sendSequenceItemInExplicitLength(B, *I);
      }

      sentlength += writeLengthAndVR(sentlength, VR_SQ);
      m_buffer.insert(m_buffer.end(), B.begin(), B.end());//length has been added in the send sq item

      return sentlength;
    }
  }

  UINT32 WriteToBuffer(const DataSet& data, Buffer& buffer, TS transfer_syntax)
  {
    Encoder E(buffer, data, transfer_syntax);
    return E.encode();
  }
}//namespace dicom
