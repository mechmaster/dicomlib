/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <iostream>

#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/tokenizer.hpp>

#include "DataSet.hpp"
#include "Types.hpp"
#include "VR.hpp"
#include "Exceptions.hpp"
#include "DataDictionary.hpp"
#include "ValueToStream.hpp"
#include "Dumper.hpp"

#include "Decoder.hpp"

namespace dicom
{
  Decoder::Decoder(Buffer& buffer, DataSet& ds, TS ts) :
    m_buffer(buffer),m_dataset(ds),m_ts(ts)
  {
  }
  
  //!DICOM messages need to have even byte length.(Part 5 section 7.1)
  void CheckEven(int ByteLength)throw (exception)
  {
    if(ByteLength & 1)
    {
      throw dicom::exception("Byte length not even.");
    }
  }

  void Decoder::decodeVRAndLength(Tag tag, VR& vr, UINT32& length)
  {
    if(m_ts.isExplicitVR()) //then get VR from stream, VR always little_endian -Sam
    {
      BYTE b1;
      BYTE b2;

      m_buffer >> b1;
      m_buffer >> b2;

      UINT16 w = (UINT16(b2) << 8) | b1;

      vr=VR(w);

      if (vr == VR_UN || vr == VR_SQ || vr == VR_OW || vr == VR_OB || vr == VR_UT)//see Part5 / 7.1.2
      {
        m_buffer >> w; // 2 bytes unused
        m_buffer >> length; //4 bytes of length info
      }
      else
      {
        m_buffer >> w; //only 2 bytes of length info
        length = w;
      }
    }
    else //VR is implicit, look up in data dictionary
    {
      vr=GetVR(tag); //look up in data dictionary.

      //This is a hack to determine if it is OW or OB in case implicit vr and tag==TAG_PIXEL_DATA -Sam
      if (tag == TAG_PIXEL_DATA)
      {
        UINT16 bits = 0;

        if (m_dataset.exists(TAG_BITS_ALLOC))
        {
          m_dataset(TAG_BITS_ALLOC) >> bits;
        }

        if (bits == 8)
        {
          vr = VR_OB;
        }
        else if (bits == 16)
        {
          vr = VR_OW;
        }
        //else, something wrong if reaching here, don't know what to do -Sam
      }

      m_buffer >> length; //4 bytes of length info.
    }
  }

  //TAG_NULL not handled?
  /*!
  This is described in Part 5, section 7.1
  */
  void Decoder::decodeElement()
  {
    Tag tag;
    m_buffer >> tag;

    if (tag == TAG_ITEM_DELIM_ITEM)
    {
      UINT32 dummy_length;
      m_buffer >> dummy_length; //deliminator item always has length 0x00000000;

      throw EndOfSequence(); //ie jump to end of loop in Decoder::Decode()
      //This is a bit of a hack, not an optimal approach to handling
      //this kind of situation, as it's not really 'unexpected'.
    }

    VR vr;
    UINT32 length;

    decodeVRAndLength(tag, vr, length);

    if(tag == TAG_NULL)
    {
      //cout<< "null tag, length=" << length << endl;
      //what am I supposed to do here?(I think this has something to do with group length, which we're ignoring)
      //I think this gets sent at the beginning of a DICOM message.  TODO
    }

    if (vr == VR_SQ || (vr == VR_UN && length == UNDEFINED_LENGTH))//See Part 5, section 6.2.2, Notes 4
    //if (vr == VR_SQ)//is this correct?
    {
      return decodeSequence(tag, length);
    }

    if (UNDEFINED_LENGTH != length)
    {
      CheckEven(length);
    }

    if(TAG_DATA_SET_PADDING == tag)
    {
      //throw away padding - we don't maintain it.
      std::vector<unsigned char> dummy(length);
      m_buffer >> dummy;

      return;
    }

    /*
    use the 'switch' statement to explicitly instantiate instances
    of GetElementValue templated on required VR.
    */
    /*
    Now we read the relevant data from the byte stream, transfrom it
    to the correct C++ type and push it onto the data set.
    */

    /*
    Part5, Annex A actually says we have to do some magic on a bunch of the 
    OB/OW tags, depending on the transfer syntax, but currently we're ignoring
    that, and letting the end user sort it out.
    */

    switch(vr)
    {
      case VR_AT://attribute tag??
        getElementValue<VR_AT>(tag, length);
        return;
      case VR_US://Unsigned Short
        getElementValue<VR_US>(tag, length);
        return;
      case VR_SS://signed short
        getElementValue<VR_SS>(tag, length);
        return;
      case VR_OB://other byte string
        return decodeOB(tag, length);
      case VR_OW://other word string
        {
        TypeFromVR<VR_OW>::Type data((length >> 1), 0);// divide length by 2 because this is an array of
                            // two-byte characters, and 'length' is the number
                            // of bytes.
        m_buffer >> data;
        m_dataset.put<VR_OW>(tag, data);
        }
        break;
      case VR_UL://unsigned long
        getElementValue<VR_UL>(tag, length);
        return;
      case VR_SL://signed long
        getElementValue<VR_SL>(tag, length);
        return;
      case VR_FL://float
        getElementValue<VR_FL>(tag, length);
        return;
      case VR_FD://double
        getElementValue<VR_FD>(tag, length);
        return;
      case VR_UI://unique identifier.
        return decodeUID(tag, length);
      case VR_DA://date
        return decodeString<VR_DA>(tag, length);
      case VR_UN:
        {
          if (length > 0)
          {
            std::vector<BYTE> v(length);
            m_buffer >> v;
            m_dataset.put<VR_UN>(tag, v);
          }
        }
        break;

      //all the string types!
      //(This list will get shorter as we introduce more intelligent types for
      // some VR's, e.g. a proper Date type,)
      case VR_AS:
        return decodeString<VR_AS>(tag, length);
      case VR_CS:
        return decodeString<VR_CS>(tag, length);
      case VR_DS:
        return decodeString<VR_DS>(tag, length);
      case VR_DT:
        return decodeString<VR_DT>(tag, length);//really should be a datetime object!!
      case VR_LO:
        return decodeString<VR_LO>(tag, length);
      case VR_LT:
        return decodeString<VR_LT>(tag, length);
      case VR_PN:
        return decodeString<VR_PN>(tag, length);
      case VR_SH:
        return decodeString<VR_SH>(tag, length);
      case VR_ST:
        return decodeString<VR_ST>(tag, length);
      case VR_TM:
        return decodeString<VR_TM>(tag, length);
      case VR_UT:
        return decodeString<VR_UT>(tag, length);
      case VR_IS:
        return decodeString<VR_IS>(tag, length);
      case VR_AE:
        return decodeString<VR_AE>(tag, length);
      default:
        std::cout << "Unknown VR: " << UINT32(vr) << " in DecodeElement()" << std::endl;
        std::cout << "Tag is: " << UINT32(tag) << "  dataset size is " << m_dataset.size() << std::endl;
        throw UnknownVR(vr);
    }
  }

  /*!
  Sequences are described in Part 5, section 7.5.
  */
  void Decoder::decodeSequence(Tag SequenceTag, UINT32 SequenceLength)
  {
    //need to keep track of bytes read if SequenceLength is not UNDEFINED_LENGTH
    Sequence sequence;
    UINT32 BytesLeftToRead = SequenceLength;

    while(BytesLeftToRead > 0)
    {
      UINT16 Group;
      UINT16 Element;
      m_buffer >>Group;
      m_buffer >>Element;
      Tag tag=makeTag(Group,Element);

      UINT32 ItemLength;
      m_buffer >> ItemLength;

      if(BytesLeftToRead!=UNDEFINED_LENGTH)
      BytesLeftToRead -= 8;
      switch (tag)
      {
      case TAG_ITEM:
      {
      DataSet data;

      if(ItemLength!=UNDEFINED_LENGTH)
      {
      /* make a copy of relevant bit of buffer.
      * This is time-intensive, but simplifies the code considerably.
      */


      Buffer b(m_buffer.GetEndian());
      std::copy(m_buffer.position(),m_buffer.position()+ItemLength,std::back_inserter(b));
      Decoder D(b,data,m_ts);
      D.decode();

      //buffer_.position()+=ItemLength;
      m_buffer.increment(ItemLength);

      if(BytesLeftToRead!=UNDEFINED_LENGTH)
      BytesLeftToRead-=ItemLength;
      }
      else
      {
      /*
      Just feed in current buffer and trust system to correctly increment I
      */

      Buffer::iterator I=m_buffer.position();
      Decoder D(m_buffer,data,m_ts);

      D.decode();


      UINT32 BytesRead=m_buffer.position()-I;

      if(BytesLeftToRead!=UNDEFINED_LENGTH)
      BytesLeftToRead-=BytesRead;

      }

      sequence.push_back(data);
      if(BytesLeftToRead==0)
      {

      m_dataset.put<VR_SQ>(SequenceTag,sequence);

      return;
      }
      }
      break;
      case TAG_SEQ_DELIM_ITEM:
      //maybe check that length is 0x00000000
      if(ItemLength!=0)
      std::cout<<"sequence delimination item length should really be zero." << std::endl;

      //we're done

      m_dataset.put<VR_SQ>(SequenceTag,sequence);

      return;
      default:
      std::cout << "some funny tag in DecodeSequence() :" << tag << std::endl;
      throw UnknownTag(tag);
      }
    }

    m_dataset.put<VR_SQ>(SequenceTag, sequence);
  }

  void Decoder::decode()
  {
    try
    {
      while (m_buffer.position() != m_buffer.end())
      {
        decodeElement();
      }
    }
    catch (EndOfSequence)
    {
      return;
    }
  }

  /**
  When an exception is caught, just skip the tag element which causes the problem
  and then jump to the next tag keeping the decode process going on.

  More catch blocks might be added.
  Mei Ge, June 2006
  */
  void Decoder::nonStandardDecode()
  {
    try
    {
      while(m_buffer.position() != m_buffer.end())
      {
        decodeElement();
      }
    }
    catch (ReadBeyondBuffer e)
    {
      std::cout << e.what() << std::endl;
      return;
    }
    catch (EndOfSequence)
    {
      return;
    }
    catch (DecoderError e)
    {
      nonStandardDecode();
    }
    catch (UnknownTag e)
    {
      nonStandardDecode();
    }
    catch (InvalidUID e)
    {
      nonStandardDecode();
    }
  }

  /** For Cumulus3, we definately need to open those files with small problems.
  * Also, it won't cause problems for you guys.
  * Mei, Nov 2006
  */
  //  void ReadFromBuffer(Buffer& buffer, DataSet& data, TS transfer_syntax)
  //  {
  //    Decoder d(buffer,data,transfer_syntax);
  //    d.NonStandardDecode();
  //  }


  /*
  Trevor.Morgan@sri.utoronto.ca /  December 2012
  For now I've removed the above hack, because I want to write a more robust mechanism
  for handling 'incorrect' files.  This should ideally be handled in the context of a user
  option; for example a 'strict' mode and a 'lax' mode for the parser.  Furthermore we 
  really need a solid suite of sample files to test against.
  */

  void ReadFromBuffer(Buffer& buffer, DataSet& data, TS transfer_syntax)
  {
    Decoder d(buffer, data, transfer_syntax);
    d.decode();
  }

  void ReadElementFromBuffer(Buffer& buffer, DataSet& ds, TS transfer_syntax)
  {
    Decoder d(buffer, ds, transfer_syntax);
    d.decodeElement();
  }
};//namespace dicom

/*

Some notes on data time:


Part5/Table 6.2-1 specifies how VR_DT is encoded.
We're not currently converting this interally to a boost time object, but
hopefully we will in the future.
However boost doesn't have a mechanism for managing Coordinated Universal Time
offsets, so it might be safest to just leave it as a string.  If we do that,
then we should probably also make VR_DA stay as a string, and get rid of
the dependency on boost::date_time entirely?

*/

