/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <iostream>
#include <string>
#include <map>
#include "socket/Base.hpp"
#include "VR.hpp"
#include "DataSet.hpp"
#include "DataDictionary.hpp"
#include "ValueToStream.hpp"
#include "iso646.h"

using namespace std;
namespace dicom
{
	namespace//everything in here is purely implementation.
	{
		//!provides human readable names for VRs
		/*!
			first entry in pair is, eg "Application Entity"
			second entry is ege "AE", useful for writing SQL to talk to dicom server
		*/
		struct VR_MAP : public map<VR,pair<string,string> >
		{
			VR_MAP()
			{
				typedef pair<string,string> PAIR;
				
				(*this)[VR_AE]=PAIR("Application Entity","AE");
				(*this) [VR_AS]=PAIR("Age String","AS");
				(*this) [VR_AT]=PAIR("Attribute Tag","AT");
				(*this) [VR_CS]=PAIR("Code String","CS");
				(*this) [VR_DA]=PAIR("Date","DA");
				(*this) [VR_DS]=PAIR("Decimal String","DS");
				(*this) [VR_DT]=PAIR("DateTime","DT");
				(*this) [VR_FL]=PAIR("Floatingpointsingle","FL");
				(*this) [VR_FD]=PAIR("Floatingpointdouble","FD");
				(*this) [VR_IS]=PAIR("Integer String","IS");
				(*this) [VR_LO]=PAIR("Longstring","LO");
				(*this) [VR_LT]=PAIR("Long Text","LT");
				(*this) [VR_OB]=PAIR("OtherByteString","OB");
				(*this) [VR_OW]=PAIR("OtherWordString","OW");
				(*this) [VR_PN]=PAIR("PersonName","PN");
				(*this) [VR_SH]=PAIR("ShortString","SH");
				(*this) [VR_SL]=PAIR("Signedlong","SL");
				(*this) [VR_SQ]=PAIR("Sequence","SQ");
				(*this) [VR_SS]=PAIR("SignedShort","SS");
				(*this) [VR_ST]=PAIR("Shorttext","ST");
				(*this) [VR_TM]=PAIR("Time","TM");
				(*this) [VR_UI]=PAIR("UniqueIdentifier","UI");
				(*this) [VR_UL]=PAIR("UnsignedLong","UL");
				(*this) [VR_UN]=PAIR("Unknown","UN");
				(*this) [VR_US]=PAIR("UnsignedShort","US");
			}
		};

		//!Is this global thread-safe?
		/*!
			See comments on TheDataDictionary in DataDictionary.cpp, where we 
			use the same pattern.
		*/

		VR_MAP TheVRMap;

		/*!
			This function may want to be a member of Value.
		*/
		template <VR vr>
		void Dump2(const Value& value,std::ostream& out)
		{
			typename dicom::TypeFromVR<vr>::Type data;
			//out << vr << ",";
			if(!value.empty())
			{
				value >> data;
				out << data;
			}
			//out << vr_map[vr] << "," << data;
		}

		template <>
		void Dump2<VR_UI>(const Value& value,std::ostream& out)
		{
			//out << VR_UI << ",";
			UID uid=value.Get<UID>();
			out << uid.str();
		}


		template<>
		void Dump2<VR_OB>(const Value& value,std::ostream& out)
		{
			typedef TypeFromVR<VR_OB>::Type Type;

			const Type& data = value.Get<Type>();//shouldn't do any copying.

			//out << VR_OB << ",";
			out << "8-bit binary data:";
			//print the first few bytes...
			//for(Type::const_iterator I = data.begin();I!=data.end() and I<data.begin()+10;I++)
			//	out << *I << ",";
			out <<  "(" <<  data.size() << " bytes)" ;
		}

		template<>
		void Dump2<VR_OW>(const Value& value,std::ostream& out)
		{
			typedef TypeFromVR<VR_OW>::Type Type;
			const Type& data = value.Get<Type>();//shouldn't do any copying.

			//out << VR_OW << ",";
			out << "16-bit binary data:";
			for(Type::const_iterator I = data.begin();I!=data.end() and I<data.begin()+10;I++)
				out << *I << ",";
			out <<  "...(" <<  data.size() << " 2 byte pairs)" ;
		}
		template<>
		void Dump2<VR_UN>(const Value& value,std::ostream& out)
		{
			//out << VR_UN << ",";
			out << "Unknown data";// of length " << value.size();
		}


		struct DumpValue
		{
			std::ostream& out;
			DumpValue(std::ostream& os):out(os){}
			void operator()(const DataSet::value_type v)
			{
				out << GetName(v.first) << "\t\t " <<
					v.second << endl;
			}
		};

		struct DumpDataSet
		{
			std::ostream& out;
			DumpDataSet(std::ostream& os):out(os){}
			void operator()(const DataSet& data)
			{
				for_each(data.begin(),data.end(),DumpValue(out));
			}
		};

		template<>
		void Dump2<VR_SQ>(const Value& value,std::ostream& out)
		{
			////need to recursively dump here, i think....
			out <<endl<<"Begin Sequence -----------------"<<endl;
			const Sequence& s = value.Get<Sequence>();

			for_each(s.begin(),s.end(),DumpDataSet(out));
			out <<"  End Sequence -----------------"<<endl;

		}

		template <>
		void Dump2<VR_AT>(const Value& value,std::ostream& out)
		{
			Tag data=value.Get<Tag>();
			out << GetName(data);
		}

		istream& operator >> (istream& In,dicom::UID& uid)
		{
			string s;
			In >> s;
			uid=UID(s);
			return In;
		}

		/*
			more specialized stuff....
		*/
		void Dump(const Value& value, std::ostream& out)
		{
			switch(value.vr())
			{
			case VR_AE:
				return Dump2<VR_AE>(value,out);
			case VR_AS:
				return Dump2<VR_AS>(value,out);
			case VR_AT:
				return Dump2<VR_AT>(value,out);
			case VR_CS:
				return Dump2<VR_CS>(value,out);
			case VR_DA:
				return Dump2<VR_DA>(value,out);
			case VR_DS:
				return Dump2<VR_DS>(value,out);
			case VR_DT:
				return Dump2<VR_DT>(value,out);
			case VR_FD:
				return Dump2<VR_FD>(value,out);
			case VR_FL:
				return Dump2<VR_FL> (value,out);
			case VR_IS:
				return Dump2<VR_IS>(value,out);
			case VR_LO:
				return Dump2<VR_LO>(value,out);
			case VR_LT:
				return Dump2<VR_LT>(value,out);
			case VR_OB:
				return Dump2<VR_OB>(value,out);
			case VR_OW:
				return Dump2<VR_OW>(value,out);
			case VR_PN:
				return Dump2<VR_PN>(value,out);
			case VR_SH:
				return Dump2<VR_SH>(value,out);
			case VR_SL:
				return Dump2<VR_SL> (value,out);
			case VR_SQ:
				return Dump2<VR_SQ>(value,out);
			case VR_SS:
				return Dump2<VR_SS> (value,out);
			case VR_ST:
				return Dump2<VR_ST>(value,out);
			case VR_TM:
				return Dump2<VR_TM>(value,out);
			case VR_UI:
				return Dump2<VR_UI>(value,out);
			case VR_UL:
				return Dump2<VR_UL>(value,out);
			case VR_UN:
				return Dump2<VR_UN>(value,out);
			case VR_US:
				return Dump2<VR_US> (value,out);
			case VR_UT:
				return Dump2<VR_UT>(value,out);
			default:
				throw BadVR(value.vr());

			}
		}



		template <VR vr>
		Value ValueFromStream(std::istream& In)
		{
			typename TypeFromVR<vr>::Type data;
			In >> data;
			return Value(vr,data);
		}


	}//namespace

	/*!
		Usage:
		std::stringstream s("Fred^Bloggs\n3.14159");
		Value v=ValueFromStream(VR_PN,s);
		Value v2=ValueFromStream(VR_FD,s);
		cout << v << endl;
		cout << v2 << endl;

		Note that the following VRs will NOT supported:
		OB,OW,SQ,UN

		DA is not currently supported.
	*/
	Value ValueFromStream(VR vr,std::istream& In)
		{
			//explicitly instantiate all supported templated functions.
			switch(vr)
			{
			case VR_AE:
				return ValueFromStream<VR_AE>(In);
			case VR_AS:
				return ValueFromStream<VR_AS>(In);
			case VR_CS:
				return ValueFromStream<VR_CS>(In);
 			case VR_DA:
 				return ValueFromStream<VR_DA>(In);
			case VR_DS:
				return ValueFromStream<VR_DS>(In);
			case VR_DT:
				return ValueFromStream<VR_DT>(In);
			case VR_FD:
				return ValueFromStream<VR_FD>(In);
			case VR_FL:
				return ValueFromStream<VR_FL> (In);
			case VR_IS:
				return ValueFromStream<VR_IS>(In);
			case VR_LO:
				return ValueFromStream<VR_LO>(In);
			case VR_LT:
				return ValueFromStream<VR_LT>(In);
			case VR_PN:
				return ValueFromStream<VR_PN>(In);
			case VR_SH:
				return ValueFromStream<VR_SH>(In);
			case VR_SL:
				return ValueFromStream<VR_SL> (In);
			case VR_SS:
				return ValueFromStream<VR_SS> (In);
			case VR_ST:
				return ValueFromStream<VR_ST>(In);
			case VR_TM:
				return ValueFromStream<VR_TM>(In);
			case VR_UI:
				return ValueFromStream<VR_UI>(In);
			case VR_UL:
				return ValueFromStream<VR_UL>(In);
			case VR_US:
				return ValueFromStream<VR_US> (In);
			case VR_UT:
				return ValueFromStream<VR_UT>(In);
			default:
				throw BadVR(vr);
			}

		}

		/*!
			typical usage:
			void Print(const Value& v)
			{
				cout << v << endl;
			}
		*/
		std::ostream& operator <<  (std::ostream& out, const Value& value)
		{
			out<<value.vr()<<",";	//NOT nice!!! Some time we don't need VR
			Dump (value,out);
			return out;
		}
		std::string GetValueDataInString(const Value& value)
		{
			ostringstream os;
			Dump(value,os);
			return os.str();
		}
		std::string GetVRName(VR vr)
		{
			return TheVRMap[vr].first;
		}

		/*!
			Converts e.g. VR_US to "US"
		*/
		std::string GetVRShortName(VR vr)
		{
			return TheVRMap[vr].second;
		}
		/*!
			Converts e.g. "US" to VR_US
		*/
		VR GetVRFromShortName(std::string strVR)
		{
			char l=strVR[0];
			char r=strVR[1];
			UINT16 v=l|(r<<8);
			return VR(v);
		}


		std::ostream& operator << (std::ostream& out, VR vr)
		{
			out << GetVRShortName(vr);
			return out;
		}

}//namespace dicom
