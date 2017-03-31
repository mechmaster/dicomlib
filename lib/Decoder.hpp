#pragma once

#include <boost/tokenizer.hpp>

#include "DataSet.hpp"
#include "TransferSyntax.hpp"
#include "Exceptions.hpp"
#include "Buffer.hpp"

/*
TODO

This file presents an unnecessarily complicated interface to the 
end user.  Pleas simplify.

  hint - The user probably doesn't need to know about Decoder
  objects - the interface could be reduced to:
Decode(Buffer&,DataSet&,TS);
*/

namespace dicom
{
  struct DecoderError : public dicom::exception
  {
    DecoderError(std::string description) :
      dicom::exception(description)
    {
    }

    virtual ~DecoderError() throw()
    {
    }
  };
  
  class Decoder
  {
    Buffer& m_buffer;
    DataSet& m_dataset;
    TS m_ts;

  public:

    Decoder(Buffer& buffer, DataSet& ds, TS ts);

    void decode();
    void decodeElement();

    //!see 5/7.5.2
    struct EndOfSequence
    {
    };

    /**
     T h*is is to handle those images with DICOM standard compliance problems. 
     
     Trevor might dislike it. However it's an unfortunate fact that not every single element
     in the header of a DICOM image always comply perfectly with the DICOM standard. 
     When the problem is not critical, the capbility of opening the image is almost always expected.
     
     Mei Ge, June 19, 2006
     */
    void nonStandardDecode();
    
  private:
    
    void decodeVRAndLength(Tag tag, VR& vr, UINT32& length);
    void decodeSequence(Tag tag, UINT32 length);
    
    /*!
     E x*tract one element onto dataset.
     */

    template <VR vr>
    void getElementValue(Tag tag)
    {
      typename TypeFromVR<vr>::Type data;
      
      m_buffer >> data;

      //the 'template' keyword in the following line is technically redundant
      //but works round a known bug in gcc.
      //See http://gcc.gnu.org/cgi-bin/gnatsweb.pl?cmd=view%20audit-trail&database=gcc&pr=9510
      m_dataset.put<vr>(tag, data);
    }
    
    /*!
     O n*ly use previous function directly if we're sure that multiplicity is 1.
     (ie. if VR is one of SQ,OB,OW,or UN).  Otherwise use this function, which
     takes multiplicity into account.
     */
    template <VR vr>
    void getElementValue(Tag tag, size_t length)
    {
      StaticMultiplicityCheck<vr>();
      typedef typename TypeFromVR<vr>::Type DataType;
      
      if(length == 0)
      {
        m_dataset.put<vr>(tag);
        return;
      }

      Buffer::iterator end = m_buffer.position() + length;
      while(m_buffer.position() != end)
      {
        getElementValue<vr>(tag);
      }
    }
    
    /*!
     s l*ightly different than above, as string multiplicity is handled differently than
     other types, using a backslash as a seperator.
     */
    
    template <VR vr>
    void decodeString(Tag tag, size_t length)
    {
      BOOST_STATIC_ASSERT((boost::is_same<std::string, typename TypeFromVR<vr>::Type>::value));

      std::string s(length, ' ');
      m_buffer >> s;
      if (length == 0) //Sam Shen put this block here to avoid missing the blank strings
      {
        m_dataset.put<vr>(tag, s);
        return;
      }
      
      StripTrailingWhitespace(s);
      
      /*
       T e*chnically speaking, we shouldn't have to do this, but there seem to be
       many people producing images with null characters (0x00) in strings.
       */
      StripTrailingNull(s);
      
      if (vr==VR_CS || vr==VR_AS || vr==VR_LT || vr==VR_ST || vr==VR_UT)
      {
        m_dataset.put<vr>(tag, s);
      }
      else
      {
        //	parse multiplicity using boost::tokenizer
        
        typedef boost::tokenizer<boost::char_separator<char>> tokenizer;
        boost::char_separator<char> sep("\\", "", boost::keep_empty_tokens);//strings are seperated by backslashes;
        tokenizer tokens(s, sep);
        
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
          m_dataset.put<vr>(tag, *tok_iter);
        }
      }
      
      /**	Question: Is the multiplicity of CS 1 for the above block?  
       M e*i June 09, 2006
       */ 
    }
    
    void decodeUID(Tag tag, size_t length)
    {
      std::string s(length, ' ');
      m_buffer >> s;

      StripTrailingNull(s);

      if (length == 0)
      {
        UID uid(s);
        m_dataset.put<VR_UI>(tag, uid);
        return;
      }
      
      /*
       W e* shouldn't have to do this, but some people are producing UIDS
       with trailing whitespace instead of NULLs, so we'll be lenient!
       */
      StripTrailingWhitespace(s);
      
      typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
      boost::char_separator<char> sep("\\","",boost::keep_empty_tokens);//UIDS are seperated by backslashes;
      tokenizer tokens(s, sep);

      for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
      {
        m_dataset.put<VR_UI>(tag, UID(*tok_iter));
      }
    }
    
    void decodeOB(Tag tag, size_t length)
    {
      if (length == UNDEFINED_LENGTH)
      {
        Enforce(TAG_PIXEL_DATA == tag, "only pixel data can be encoded");
        /*
         t h*is probably means we're decoding encapsulated compressed
         pixel data, see Part 5, Annex A.4.
         -Transfer syntax should be one of the JPEG transfer syntaxes,
         -tag should be 'Pixel Data'
         -Data is encapsulated as shown in Part5, Table A.4-2
         
         I think we could handle this by treating the fragments as multiple values
         for this tag.  We could throw away the offset table, sacrificing potential
           speed gains for simplicity, but this might upset clients that expect
           it to be persisted?
           */
        Enforce(m_ts.isEncapsulated(), "Undefined value length on non-encoded transfer syntax.");
        
        //extract offset table, then pull out multiple elements until we hit a Sequence Delimiter Item
        Tag offset_table_tag;
        m_buffer >> offset_table_tag;

        Enforce(TAG_ITEM == offset_table_tag, "Offset table must be defined in encoded data");
        UINT32 length;
        m_buffer >> length;
        m_buffer.increment(length);//we ignore the offset table...

        for(;;)
        {
          Tag tag;
          UINT32 length;
          m_buffer >> tag;
          m_buffer >> length;

          if (TAG_SEQ_DELIM_ITEM == tag)
          {
            break;
          }

          Enforce(TAG_ITEM == tag, "Tag must be sequence item");

          TypeFromVR<VR_OB>::Type data(length, 0);
          m_buffer >> data;
          m_dataset.put<VR_OB>(TAG_PIXEL_DATA, data);
        }
      }
      else
      {
        TypeFromVR<VR_OB>::Type data(length, 0);
        m_buffer >> data;
        m_dataset.put<VR_OB>(tag, data);
      }
    }
  };

  //!This function seems only to be used by FileMetaInformation
  void ReadElementFromBuffer(Buffer& buffer, DataSet& data, TS transfer_syntax);
  void ReadFromBuffer(Buffer& buffer, DataSet& data, TS transfer_syntax);
}//namespace dicom
