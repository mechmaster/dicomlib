/*
	DICOMLIB
	Copyright 2003 Sunnybrook and Women's College Health Science Center
	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)

	See the file LICENSE.txt for copyright and licensing info.
*/

/*
	VR.hpp
	This file explicitly defines the relationships between DICOM value
	representations ('VR') and c++ types.
*/






#ifndef VR_HPP_INCLUDE_GUARD_5645353874
#define VR_HPP_INCLUDE_GUARD_5645353874

#include <sstream>
#include <vector>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>
#include <boost/shared_ptr.hpp>

#include "Exceptions.hpp"
#include "UID.hpp"
#include "Tag.hpp"
#include "Types.hpp"

namespace dicom
{

	/*!
		Represents data of vr type 'AE'
		see section 5,table 6.2-1
	*/
	typedef std::string ApplicationEntity;/*eventually should be replaced with a strict checking class.*/

	//!Value Representations.
	/*!
		Defined in  Part 5, section 6.2 
	*/
	enum VR {
		VR_AE = 0x4541,	//!< Application Entity
	 	VR_AS = 0x5341, //!< Age String
		VR_AT = 0x5441, //!< Attribute Tag
		VR_CS = 0x5343, //!< Code String
		VR_DA = 0x4144, //!< Date
		VR_DS = 0x5344, //!< Decimal String
		VR_DT = 0x5444, //!< Date Time
		VR_FD = 0x4446, //!< Floating point double
		VR_FL = 0x4C46, //!< Floating point single
		VR_IS = 0x5349, //!< Integer String
		VR_LO = 0x4f4c, //!< Long string
		VR_LT = 0x544c, //!< Long Text
		VR_OB = 0x424f, //!< Other Byte String
		VR_OW = 0x574f, //!< Other Word String
		VR_PN = 0x4e50, //!< Person Name
		VR_SH = 0x4853, //!< Short String
		VR_SL = 0x4C53, //!< Signed long
		VR_SQ = 0x5153, //!< Sequence
		VR_SS = 0x5353, //!< Signed Short
		VR_ST = 0x5453, //!< Short text
		VR_TM = 0x4d54, //!< Time
		VR_UI = 0x4955, //!< Unique Identifier
		VR_UL = 0x4C55, //!< Unsigned Long
		VR_UN = 0x4e55, //!< Unknown
		VR_US = 0x5355, //!< Unsigned Short
		VR_UT = 0x5455  //!< Unlimited Text
		};

	//!General VR mismatch, mistake etc.
	/*!
		straighten up this class please.
	*/
	struct BadVR:public dicom::exception
	{
		//Tag tag_;
		VR vr_;
		//BadVR(Tag tag): dicom::exception("Bad VR"),tag_(tag){}
		BadVR(VR vr):dicom::exception("Bad VR"),vr_(vr){}

		virtual ~BadVR() throw(){}
		const char* what() const throw()
		{
			std::ostringstream out;
			out << "VR " << std::hex << vr_ << "  is not valid in the given context";
			return out.str().c_str();
		}
	};
	//!System got passed a value that isn't a valid VR or we don't know how to handle...
	struct UnknownVR:public dicom::exception
	{
		UINT32 vr_;
		UnknownVR():dicom::exception("Unknown VR"),vr_(0)
		{
		}
		UnknownVR(UINT32 vr):dicom::exception("Unknown VR"),vr_(vr){}
	};

	//!Retrieval of c++ type from DICOM Value Representation
	/*!
		Specialisations of this structure formally state the relationship
		between Dicom Value Representations (VR) and C++ types.

		Currently there is a 1:N relationship VR:Type.  It might make life easier
		if we had a 1:1 relationship, that is, each VR had a _unique_ type associated
		with it.  (this would require the various string vrs to be specialised carefully.)

		Then the Value class could automagically deduce the VR from the type passed to it.
		I'm not at all sure that this would be a good thing.

		The default instantiation of TypeFromVR will not compile (and will hopefully produce a vaguely
		useful compile error message), which implies that you enquired about
		a VR that the library doesn't recognise/support.


		Currently we are using std::string to do a lot of duty.  In some places it
		could be beneficial to replace it with a more specialized class that performs
		more rigourous bounds checking etc, for dicom string vrs that have a maximum length.
	*/

	template <VR vr>
	struct	TypeFromVR
	{
		//you tried asking for the type associated with a VR that I
		//don't know about.
//#if (defined _MSC_VER && _MSC_VER<=1300)//MSVC for some reason always tries to instantiate this?
//#else
//		BOOST_STATIC_ASSERT(false);//to flag the error a bit more helpfully.
//#endif
	};

	//do i need this?  Is there any platform where the following is not the case?
	BOOST_STATIC_ASSERT(sizeof(double)==8);
	BOOST_STATIC_ASSERT(sizeof(float)==4);
	
    //BOOST_STATIC_ASSERT(sizeof(signed long)==4);
	
    BOOST_STATIC_ASSERT(sizeof(signed short)==2);

	template<> struct TypeFromVR<VR_AE>{typedef	ApplicationEntity			Type;	};
	template<> struct TypeFromVR<VR_AS>{typedef	std::string					Type;	};//please replace

	//what about VR_AT ????
	template<> struct TypeFromVR<VR_AT>{typedef Tag							Type;	};

	template<> struct TypeFromVR<VR_CS>{typedef	std::string					Type;	};
    template<> struct TypeFromVR<VR_DA>{typedef std::string                 Type;   };

	template<> struct TypeFromVR<VR_DS>{typedef	std::string					Type;	};//Please FIXME
	template<> struct TypeFromVR<VR_DT>{typedef	std::string					Type;	};//please replace
	template<> struct TypeFromVR<VR_FD>{typedef	double						Type;	};
	template<> struct TypeFromVR<VR_FL>{typedef	float						Type;	};
	template<> struct TypeFromVR<VR_IS>{typedef	std::string					Type;	};//please replace
	template<> struct TypeFromVR<VR_LO>{typedef	std::string					Type;	};
	template<> struct TypeFromVR<VR_LT>{typedef	std::string					Type;	};
	template<> struct TypeFromVR<VR_OB>{typedef	std::vector<BYTE>			Type;	};
	template<> struct TypeFromVR<VR_OW>{typedef	std::vector<UINT16>			Type;	};
	template<> struct TypeFromVR<VR_PN>{typedef	std::string					Type;	};
	template<> struct TypeFromVR<VR_SH>{typedef	std::string					Type;	};//please replace.
	template<> struct TypeFromVR<VR_SL>{typedef	signed int					Type;	};//Here we use int not long as long is sometimes 64 bits, and VR_SL _must_ be 32 bits
	template<> struct TypeFromVR<VR_SS>{typedef	signed short				Type;	};

	class DataSet;//we need this forward declaration to make type checking work for sequences.
	//class Sequence;
	template<> struct TypeFromVR<VR_SQ>{typedef	std::vector<DataSet>		Type;	};

	template<> struct TypeFromVR<VR_ST>{typedef	std::string					Type;	};
	template<> struct TypeFromVR<VR_TM>{typedef	std::string					Type;	};//please replace.
	template<> struct TypeFromVR<VR_UI>{typedef	UID							Type;	};
	template<> struct TypeFromVR<VR_UL>{typedef UINT32			 			Type;	};
	template<> struct TypeFromVR<VR_UN>{typedef	std::vector<BYTE>			Type;	};// a string of bytes, don't know what else to do with it
	template<> struct TypeFromVR<VR_US>{typedef	UINT16						Type;	};
	template<> struct TypeFromVR<VR_UT>{typedef	std::string					Type;	};
	/*
		//are there any left?
		.
		.
		.
	*/



	//!static validation of VR <--> c++ type correspondance.
	/*!
		Will not compile if GIVEN_TYPE is not the correct c++ type for
		the provide VR.
	*/
	template <typename GIVEN_TYPE,VR vr>
	struct StaticVRCheck
	{
		typedef typename TypeFromVR<vr>::Type EXPECTED_TYPE;
		BOOST_STATIC_ASSERT((boost::is_same<EXPECTED_TYPE,typename boost::remove_const<GIVEN_TYPE>::type>::value));
	};

	//!Fails to compile if VR is one of SQ,OB,OW,or UN
	/*!
		See Part 5, Section 6.4
		See also specific limitations specified in table 6.2-1
	*/
	template<VR vr>
	struct StaticMultiplicityCheck
	{
		BOOST_STATIC_ASSERT(vr!= VR_SQ && vr!=VR_OB && vr!=VR_OW && vr!=VR_UN && vr!=VR_UT && vr!=VR_CS && vr!=VR_AS && vr!=VR_LT);
	};

	//!Throws BadVR if TYPE1 is not the same as TYPE2
	template<typename TYPE1, typename TYPE2>
	void ThrowIfDifferent(VR vr)
	{
		if(!(::boost::is_same<TYPE1,TYPE2>::value))
		{
			throw BadVR(vr);
		}
	}


	//!Run time check of VR <--> C++ type correspondance
	/*!
		throws BadVR if TYPE is not the correct C++ type
		for the given Value Representation.
		It is suggested to only use this when the VR cannot be
		known at compile time (when StaticVRCheck() can be used.)


	*/
	template <typename TYPE>
	void DynamicVRCheck(VR vr) throw(BadVR)
	{
		switch (vr)
		{
		case VR_AE:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_AE>::Type>(vr);
			break;
		case VR_AS:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_AS>::Type>(vr);
			break;
		case VR_AT:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_AT>::Type>(vr);
			break;
		case VR_CS:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_CS>::Type>(vr);
			break;
		case VR_DA:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_DA>::Type>(vr);
			break;
		case VR_DS:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_DS>::Type>(vr);
			break;
		case VR_DT:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_DT>::Type>(vr);
			break;
		case VR_FD:
			ThrowIfDifferent <TYPE,TypeFromVR<VR_FD>::Type>(vr);
			break;
		case VR_FL:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_FL>::Type> (vr);
			break;
		case VR_IS:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_IS>::Type>(vr);
			break;
		case VR_LO:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_LO>::Type>(vr);
			break;
		case VR_LT:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_LT>::Type>(vr);
			break;
		case VR_OB:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_OB>::Type>(vr);
			break;
		case VR_OW:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_OW>::Type>(vr);
			break;
		case VR_PN:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_PN>::Type>(vr);
			break;
		case VR_SH:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_SH>::Type>(vr);
			break;
		case VR_SL:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_SL>::Type> (vr);
			break;

		case VR_SQ:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_SQ>::Type> (vr);
			break;
		case VR_SS:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_SS>::Type> (vr);
			break;
		case VR_ST:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_ST>::Type>(vr);
			break;
		case VR_TM:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_TM>::Type>(vr);
			break;
		case VR_UI:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_UI>::Type>(vr);
			break;
		case VR_UL:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_UL>::Type>(vr);
			break;
		case VR_UN:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_UN>::Type>(vr);
			break;
		case VR_US:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_US>::Type> (vr);
			break;
		case VR_UT:
			ThrowIfDifferent<TYPE,TypeFromVR<VR_UT>::Type>(vr);
			break;
		default:

			throw BadVR(vr);
		}
	}

/*
	NOTE
	The above PATTERN (calling switch(vr) and instantiating a template
	specialisation for each known vr) occurs at least 4 times throughout
	the codebase.  Does this imply that we can rationalise it somehow,
	(without making the code truly incomprehensible?)

*/




}//namespace DICOM

#endif //VR_HPP_INCLUDE_GUARD_5645353874
