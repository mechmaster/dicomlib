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
#include "Decoder.hpp"
#include "VR.hpp"
#include "Exceptions.hpp"
#include "DataDictionary.hpp"
#include "ValueToStream.hpp"

#include "Dumper.hpp"

using namespace std;

namespace dicom{

	struct Decoder
	{
		void Decode();
		void DecodeElement();

		Decoder(Buffer& buffer,DataSet& ds,TS ts):buffer_(buffer),dataset_(ds),ts_(ts){}

		//!see 5/7.5.2
		struct EndOfSequence{};

		/**
			This is to handle those images with DICOM standard compliance problems. 

			Trevor might dislike it. However it's an unfortunate fact that not every single element
			in the header of a DICOM image always comply perfectly with the DICOM standard. 
			When the problem is not critical, the capbility of opening the image is almost always expected.

			Mei Ge, June 19, 2006
		*/
		void NonStandardDecode();

	private:

		void DecodeVRAndLength(Tag tag, VR& vr, UINT32& length);

		Buffer& buffer_;
		DataSet& dataset_;
		TS ts_;

		void DecodeSequence(Tag tag, UINT32  length);

		/*!
			Extract one element onto dataset.
		*/
		template <VR vr>
		void GetElementValue(Tag tag)
		{
			typename TypeFromVR<vr>::Type data;

			buffer_>>data;

			//the 'template' keyword in the following line is technically redundant
			//but works round a known bug in gcc.
			//See http://gcc.gnu.org/cgi-bin/gnatsweb.pl?cmd=view%20audit-trail&database=gcc&pr=9510
			dataset_.template Put<vr>(tag,data);

		}

		/*!
			Only use previous function directly if we're sure that multiplicity is 1.
			(ie. if VR is one of SQ,OB,OW,or UN).  Otherwise use this function, which
			takes multiplicity into account.
		*/
		template <VR vr>
		void GetElementValue(Tag tag, size_t length)
		{
			StaticMultiplicityCheck<vr>();
			typedef typename TypeFromVR<vr>::Type DataType;

			if(length==0)
			{
				dataset_.template Put<vr>(tag);
				return;
			}
			Buffer::iterator end = buffer_.position()+length;
			while(buffer_.position()!=end)
				GetElementValue<vr>(tag);
		}

		

		/*!
			slightly different than above, as string multiplicity is handled differently than
			other types, using a backslash as a seperator.
		*/

		template <VR vr>
		void DecodeString(Tag tag, size_t length)
		{
			BOOST_STATIC_ASSERT((boost::is_same<std::string,typename TypeFromVR<vr>::Type>::value));

			string s(length,' ');
			buffer_>>s;
			if (length == 0) //Sam Shen put this block here to avoid missing the blank strings
			{
				dataset_.template Put<vr>(tag,s);
				return;
			}

			StripTrailingWhitespace(s);
			
			/*
				Technically speaking, we shouldn't have to do this, but there seem to be
				many people producing images with null characters (0x00) in strings.
			*/
			StripTrailingNull(s);

			if (vr==VR_CS || vr==VR_AS || vr==VR_LT || vr==VR_ST || vr==VR_UT)
				dataset_.template Put<vr>(tag,s);
			else
			{
			//	parse multiplicity using boost::tokenizer

				typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
				boost::char_separator<char> sep("\\","",boost::keep_empty_tokens);//strings are seperated by backslashes;
				tokenizer tokens(s, sep);

				for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
					dataset_.template Put<vr>(tag,*tok_iter);
			}

			/**	Question: Is the multiplicity of CS 1 for the above block?  
				Mei June 09, 2006
			*/ 
		}

		void DecodeUID(Tag tag, size_t length)
		{
			string s(length, ' ');
			buffer_ >> s;
			StripTrailingNull(s);
			if(length==0)
			{
				UID uid(s);
				//dataset_.template Put<VR_UI>(tag,uid);
				dataset_.Put<VR_UI>(tag,uid);
				return;
			}

			/*
				We shouldn't have to do this, but some people are producing UIDS
				with trailing whitespace instead of NULLs, so we'll be lenient!
			*/
			StripTrailingWhitespace(s);


			typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
			boost::char_separator<char> sep("\\","",boost::keep_empty_tokens);//UIDS are seperated by backslashes;
			tokenizer tokens(s, sep);

			for (tokenizer::iterator tok_iter = tokens.begin();tok_iter != tokens.end(); ++tok_iter)
				//dataset_.template Put<VR_UI>(tag,UID(*tok_iter));
				dataset_.Put<VR_UI>(tag,UID(*tok_iter));
		}



		void DecodeOB(Tag tag, size_t length)
		{
			if(length == UNDEFINED_LENGTH)
			{
				Enforce(TAG_PIXEL_DATA==tag,"only pixel data can be encoded");
				/*
					this probably means we're decoding encapsulated compressed
					pixel data, see Part 5, Annex A.4.
					-Transfer syntax should be one of the JPEG transfer syntaxes,
					-tag should be 'Pixel Data'
					-Data is encapsulated as shown in Part5, Table A.4-2

					I think we could handle this by treating the fragments as multiple values
					for this tag.  We could throw away the offset table, sacrificing potential
					speed gains for simplicity, but this might upset clients that expect
					it to be persisted?
				*/
				Enforce(ts_.isEncapsulated(),"Undefined value length on non-encoded transfer syntax.");

				//extract offset table, then pull out multiple elements until we hit a Sequence Delimiter Item
				Tag offset_table_tag;
				buffer_ >> offset_table_tag;
				Enforce(TAG_ITEM==offset_table_tag,"Offset table must be defined in encoded data");
				UINT32 length;
				buffer_ >> length;
				buffer_.increment(length);//we ignore the offset table...

				for(;;)
				{
					Tag tag;
					UINT32 length;
					buffer_ >> tag;
					buffer_ >> length;
					if(TAG_SEQ_DELIM_ITEM==tag)
						break;
					Enforce(TAG_ITEM==tag,"Tag must be sequence item");
					TypeFromVR<VR_OB>::Type data(length,0);
					buffer_>>data;
					dataset_.Put<VR_OB>(TAG_PIXEL_DATA,data);
				}
			}
			else
			{
				TypeFromVR<VR_OB>::Type data(length,0);
				buffer_>>data;
				dataset_.Put<VR_OB>(tag,data);
			}
		}


	};



	//!DICOM messages need to have even byte length.(Part 5 section 7.1)
	void CheckEven(int ByteLength)throw (exception)
	{
		if(ByteLength & 1)
			throw dicom::exception("Byte length not even.");
	}



	void Decoder::DecodeVRAndLength(Tag tag, VR& vr, UINT32& length)
	{
		if(ts_.isExplicitVR())		//then get VR from stream, VR always little_endian -Sam
		{
			BYTE b1,b2;
			buffer_ >> b1;
			buffer_ >> b2;
			UINT16 w = (UINT16(b2)<<8)|b1;
			
			vr=VR(w);

			if (vr == VR_UN || vr == VR_SQ || vr == VR_OW || vr == VR_OB || vr == VR_UT)//see Part5 / 7.1.2
			{
				buffer_ >> w;		//	2 bytes unused
				buffer_>>length;	//4 bytes of length info
			}
			else
			{
				buffer_>>w;			//only 2 bytes of length info
				length = w;
			}
		}
		else						//VR is implicit, look up in data dictionary
		{
			vr=GetVR(tag);		//look up in data dictionary.

			//This is a hack to determine if it is OW or OB in case implicit vr and tag==TAG_PIXEL_DATA -Sam
			if(tag==TAG_PIXEL_DATA)
			{
				UINT16 bits=0;
				if(dataset_.exists(TAG_BITS_ALLOC))
					dataset_(TAG_BITS_ALLOC)>>bits;
				if(bits==8)
					vr = VR_OB;
				else if(bits==16)
					vr = VR_OW;
				//else, something wrong if reaching here, don't know what to do -Sam
			}
			buffer_ >> length;		//4 bytes of length info.
		}
	}


	//TAG_NULL not handled?
	/*!
		This is described in Part 5, section 7.1
	*/
	void Decoder::DecodeElement()
	{
		
		
		Tag tag;
		buffer_ >> tag;

		if(tag==TAG_ITEM_DELIM_ITEM)
		{
			UINT32 dummy_length;
			buffer_>> dummy_length;		//deliminator item always has length 0x00000000;

			throw EndOfSequence();	//ie jump to end of loop in Decoder::Decode()
									//This is a bit of a hack, not an optimal approach to handling
									//this kind of situation, as it's not really 'unexpected'.
		}

		VR vr;
		UINT32 length;

		DecodeVRAndLength(tag,vr,length);

		if(tag==TAG_NULL)
		{
			//cout<< "null tag, length=" << length << endl;
			//what am I supposed to do here?(I think this has something to do with group length, which we're ignoring)
			//I think this gets sent at the beginning of a DICOM message.  TODO
		}


		if (vr == VR_SQ || (vr==VR_UN && length == UNDEFINED_LENGTH))//See Part 5, section 6.2.2, Notes 4
		//if (vr == VR_SQ)//is this correct?
		{
			return DecodeSequence(tag,length);
		}

		if(UNDEFINED_LENGTH!=length)
			CheckEven(length);


		if(TAG_DATA_SET_PADDING==tag)
		{//throw away padding - we don't maintain it.
			vector<unsigned char> dummy(length);
			buffer_>>dummy;
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
			GetElementValue<VR_AT>(tag,length);
			return;
		case VR_US://Unsigned Short
			GetElementValue<VR_US>(tag,length);
			return;
		case VR_SS://signed short
			GetElementValue<VR_SS>(tag,length);
			return;
		case VR_OB://other byte string
			return DecodeOB(tag,length);
		case VR_OW://other word string
			{
				TypeFromVR<VR_OW>::Type data((length>>1),0);// divide length by 2 because this is an array of
															// two-byte characters, and 'length' is the number
															// of bytes.
				buffer_>>data;
				dataset_.Put<VR_OW>(tag,data);
			}
			break;
		case VR_UL://unsigned long
			GetElementValue<VR_UL>(tag,length);
			return;
		case VR_SL://signed long
			GetElementValue<VR_SL>(tag,length);
			return;
		case VR_FL://float
			GetElementValue<VR_FL>(tag,length);
			return;
		case VR_FD://double
			GetElementValue<VR_FD>(tag,length);
			return;
		case VR_UI://unique identifier.
			return DecodeUID(tag,length);
		case VR_DA://date
            return DecodeString<VR_DA>(tag,length);
		case VR_UN:
			{
				if(length>0)///////////////////////////////////////////////////
				{
					vector <BYTE> v (length);
					buffer_>>v;
					dataset_.Put<VR_UN>(tag,v);
				}
			}
			break;


			//all the string types!
			//(This list will get shorter as we introduce more intelligent types for
			// some VR's, e.g. a proper Date type,)
		case VR_AS:
			return DecodeString<VR_AS>(tag,length);
		case VR_CS:
			return DecodeString<VR_CS>(tag,length);
		case VR_DS:
			return DecodeString<VR_DS>(tag,length);
		case VR_DT:
			return DecodeString<VR_DT>(tag,length);//really should be a datetime object!!
		case VR_LO:
			return DecodeString<VR_LO>(tag,length);
		case VR_LT:
			return DecodeString<VR_LT>(tag,length);
		case VR_PN:
			return DecodeString<VR_PN>(tag,length);
		case VR_SH:
			return DecodeString<VR_SH>(tag,length);
		case VR_ST:
			return DecodeString<VR_ST>(tag,length);
		case VR_TM:
			return DecodeString<VR_TM>(tag,length);
		case VR_UT:
			return DecodeString<VR_UT>(tag,length);
		case VR_IS:
			return DecodeString<VR_IS>(tag,length);
		case VR_AE:
			return DecodeString<VR_AE>(tag,length);
		default:
			cout << "Unknown VR: " << UINT32(vr) << " in DecodeElement()" << endl;
			cout << "Tag is: " << UINT32(tag) << "  dataset size is " << dataset_.size() << endl;
			throw UnknownVR(vr);
		}
	}


	/*!
		Sequences are described in Part 5, section 7.5.
	*/
	void Decoder::DecodeSequence(Tag SequenceTag,UINT32 SequenceLength)
	{
		//need to keep track of bytes read if SequenceLength is not UNDEFINED_LENGTH
		Sequence sequence;
		UINT32 BytesLeftToRead=SequenceLength;
		while(BytesLeftToRead>0)
		{

			UINT16 Group;
			UINT16 Element;
			buffer_>>Group;
			buffer_>>Element;
			Tag tag=makeTag(Group,Element);

			UINT32 ItemLength;
			buffer_ >> ItemLength;

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


						Buffer b(buffer_.GetEndian());
						std::copy(buffer_.position(),buffer_.position()+ItemLength,std::back_inserter(b));
						Decoder D(b,data,ts_);
						D.Decode();

						//buffer_.position()+=ItemLength;
						buffer_.increment(ItemLength);

						if(BytesLeftToRead!=UNDEFINED_LENGTH)
							BytesLeftToRead-=ItemLength;
					}
					else
					{
						/*
						Just feed in current buffer and trust system to correctly increment I
						*/

						Buffer::iterator I=buffer_.position();
						Decoder D(buffer_,data,ts_);

						D.Decode();


						UINT32 BytesRead=buffer_.position()-I;

						if(BytesLeftToRead!=UNDEFINED_LENGTH)
							BytesLeftToRead-=BytesRead;

					}

					sequence.push_back(data);
					if(BytesLeftToRead==0)
					{

						dataset_.Put<VR_SQ>(SequenceTag,sequence);

						return;
					}
				}
				break;
			case TAG_SEQ_DELIM_ITEM:
				//maybe check that length is 0x00000000
				if(ItemLength!=0)
					cout<<"sequence delimination item length should really be zero." << endl;

				//we're done

				dataset_.Put<VR_SQ>(SequenceTag,sequence);

				return;
			default:
				cout << "some funny tag in DecodeSequence() :" << tag << endl;
				throw UnknownTag(tag);
			}
		}

		dataset_.Put<VR_SQ>(SequenceTag,sequence);
	}

	void Decoder::Decode()
	{
		try
		{
			while(buffer_.position()!=buffer_.end())
				DecodeElement();
		}
		catch(EndOfSequence)
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
	void Decoder::NonStandardDecode()
	{
		try
		{
			while(buffer_.position()!=buffer_.end())
				DecodeElement();
		}
		catch(ReadBeyondBuffer e)
		{
			cout<<e.what()<<endl;
			return;
		}
		catch(EndOfSequence)
		{
			return;
		}
		catch(DecoderError e)
		{
			NonStandardDecode();
		}
		catch(UnknownTag e)
		{
			NonStandardDecode();
		}
		catch(InvalidUID e)
		{
			NonStandardDecode();
		}
	}

	/** For Cumulus3, we definately need to open those files with small problems.
	  * Also, it won't cause problems for you guys.
	  * Mei, Nov 2006
	  */
//	void ReadFromBuffer(Buffer& buffer, DataSet& data, TS transfer_syntax)
//	{
//		Decoder d(buffer,data,transfer_syntax);
//		d.NonStandardDecode();
//	}


/*
    Trevor.Morgan@sri.utoronto.ca /  December 2012
    For now I've removed the above hack, because I want to write a more robust mechanism
    for handling 'incorrect' files.  This should ideally be handled in the context of a user
    option; for example a 'strict' mode and a 'lax' mode for the parser.  Furthermore we 
    really need a solid suite of sample files to test against.
*/

	void ReadFromBuffer(Buffer& buffer, DataSet& data, TS transfer_syntax)
	{
		Decoder d(buffer,data,transfer_syntax);
		d.Decode();
	}

	void ReadElementFromBuffer(Buffer& buffer, DataSet& ds,TS transfer_syntax)
	{
		Decoder d(buffer,ds,transfer_syntax);
		d.DecodeElement();

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

