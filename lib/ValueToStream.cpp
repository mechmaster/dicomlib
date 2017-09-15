/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <map>
#include <utility>
#include <functional>

#include "DataSet.hpp"
#include "DataDictionary.hpp"
#include "ValueToStream.hpp"
//#include "iso646.h"

namespace dicom
{
  namespace//everything in here is purely implementation.
  {
    //!provides human readable names for VRs
    /*!
    first entry in pair is, eg "Application Entity"
    second entry is ege "AE", useful for writing SQL to talk to dicom server
    */

    static const std::map<VR, std::pair<std::string, std::string>> TheVRMap =
    {
      {VR_AE, std::make_pair("Application Entity", "AE")},
      {VR_AS, std::make_pair("Age String", "AS")},
      {VR_AT, std::make_pair("Attribute Tag", "AT")},
      {VR_CS, std::make_pair("Code String", "CS")},
      {VR_DA, std::make_pair("Date", "DA")},
      {VR_DS, std::make_pair("Decimal String", "DS")},
      {VR_DT, std::make_pair("DateTime", "DT")},
      {VR_FL, std::make_pair("Floatingpointsingle", "FL")},
      {VR_FD, std::make_pair("Floatingpointdouble", "FD")},
      {VR_IS, std::make_pair("Integer String", "IS")},
      {VR_LO, std::make_pair("Longstring", "LO")},
      {VR_LT, std::make_pair("Long Text", "LT")},
      {VR_OB, std::make_pair("OtherByteString", "OB")},
      {VR_OW, std::make_pair("OtherWordString", "OW")},
      {VR_PN, std::make_pair("PersonName", "PN")},
      {VR_SH, std::make_pair("ShortString", "SH")},
      {VR_SL, std::make_pair("Signedlong", "SL")},
      {VR_SQ, std::make_pair("Sequence", "SQ")},
      {VR_SS, std::make_pair("SignedShort", "SS")},
      {VR_ST, std::make_pair("Shorttext", "ST")},
      {VR_TM, std::make_pair("Time", "TM")},
      
      {VR_UI, std::make_pair("UniqueIdentifier", "UI")},
      {VR_UL, std::make_pair("UnsignedLong", "UL")},
      {VR_UN, std::make_pair("Unknown", "UN")},
      {VR_US, std::make_pair("UnsignedShort", "US")},
    };

    /*!
    This function may want to be a member of Value.
    */
    template <VR vr>
    void Dump2(const Value& value, std::ostream& out)
    {
      typename dicom::TypeFromVR<vr>::Type data;

      //out << vr << ",";
      if(!value.empty())
      {
        value >> data;
        out << data;
      }
    }

    template <>
    void Dump2<VR_UI>(const Value& value, std::ostream& out)
    {
      //out << VR_UI << ",";
      UID uid = value.Get<UID>();
      out << uid.str();
    }

    template<>
    void Dump2<VR_OB>(const Value& value, std::ostream& out)
    {
      typedef TypeFromVR<VR_OB>::Type Type;
      const Type& data = value.Get<Type>();//shouldn't do any copying.

      //out << VR_OB << ",";
      out << "8-bit binary data:";
      for(Type::const_iterator iter = data.begin(); iter != data.end() && iter < data.begin() + 10; ++iter)
      {
        out << std::hex << *iter << ",";
      }
      out << "(" <<  data.size() << " bytes)";
    }

    template<>
    void Dump2<VR_OW>(const Value& value, std::ostream& out)
    {
      typedef TypeFromVR<VR_OW>::Type Type;
      const Type& data = value.Get<Type>();//shouldn't do any copying.

      //out << VR_OW << ",";
      out << "16-bit binary data:";
      for(Type::const_iterator iter = data.begin(); iter != data.end() && iter < data.begin() + 10; ++iter)
      {
        out << std::hex << *iter << ",";
      }
      out <<  "...(" <<  data.size() << " 2 byte pairs)" ;
    }

    template<>
    void Dump2<VR_UN>(const Value& value, std::ostream& out)
    {
      //out << VR_UN << ",";
      out << "Unknown data";// of length " << value.size();
    }
    
    void DumpValue(const DataSet::value_type v, std::ostream& out)
    {
      out << GetName(v.first) << "\t\t " << v.second << std::endl;      
    }

    void DumpDataSet(const DataSet& data, std::ostream& out)
    {
      std::for_each(data.begin(), data.end(), std::bind(DumpValue, std::placeholders::_1, std::ref(out)));
    }

    template<>
    void Dump2<VR_SQ>(const Value& value, std::ostream& out)
    {
      ////need to recursively dump here, i think....
      out << std::endl <<"Begin Sequence -----------------" << std::endl;
      const Sequence& s = value.Get<Sequence>();

      std::for_each(s.begin(), s.end(), std::bind(DumpDataSet, std::placeholders::_1, std::ref(out)));
      out << "  End Sequence -----------------" << std::endl;
    }

    template <>
    void Dump2<VR_AT>(const Value& value, std::ostream& out)
    {
      Tag data = value.Get<Tag>();
      out << GetName(data);
    }

    std::istream& operator >> (std::istream& In, dicom::UID& uid)
    {
      std::string s;
      In >> s;
      uid = UID(s);
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
          return Dump2<VR_AE>(value, out);
        case VR_AS:
          return Dump2<VR_AS>(value, out);
        case VR_AT:
          return Dump2<VR_AT>(value, out);
        case VR_CS:
          return Dump2<VR_CS>(value, out);
        case VR_DA:
          return Dump2<VR_DA>(value, out);
        case VR_DS:
          return Dump2<VR_DS>(value, out);
        case VR_DT:
          return Dump2<VR_DT>(value, out);
        case VR_FD:
          return Dump2<VR_FD>(value, out);
        case VR_FL:
          return Dump2<VR_FL> (value, out);
        case VR_IS:
          return Dump2<VR_IS>(value, out);
        case VR_LO:
          return Dump2<VR_LO>(value, out);
        case VR_LT:
          return Dump2<VR_LT>(value, out);
        case VR_OB:
          return Dump2<VR_OB>(value, out);
        case VR_OW:
          return Dump2<VR_OW>(value, out);
        case VR_PN:
          return Dump2<VR_PN>(value, out);
        case VR_SH:
          return Dump2<VR_SH>(value, out);
        case VR_SL:
          return Dump2<VR_SL> (value, out);
        case VR_SQ:
          return Dump2<VR_SQ>(value, out);
        case VR_SS:
          return Dump2<VR_SS> (value, out);
        case VR_ST:
          return Dump2<VR_ST>(value, out);
        case VR_TM:
          return Dump2<VR_TM>(value, out);
        case VR_UI:
          return Dump2<VR_UI>(value, out);
        case VR_UL:
          return Dump2<VR_UL>(value, out);
        case VR_UN:
          return Dump2<VR_UN>(value, out);
        case VR_US:
          return Dump2<VR_US> (value, out);
        case VR_UT:
          return Dump2<VR_UT>(value, out);
        default:
          throw BadVR(value.vr());
      }
    }

    template <VR vr>
    Value ValueFromStream(std::istream& In)
    {
      typename TypeFromVR<vr>::Type data;
      In >> data;
      return Value(vr, data);
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
  Value ValueFromStream(VR vr, std::istream& In)
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
    out << value.vr() << ","; //NOT nice!!! Some time we don't need VR
    Dump(value, out);
    return out;
  }

  std::string GetValueDataInString(const Value& value)
  {
    std::ostringstream os;
    Dump(value,os);
    return os.str();
  }

  std::string GetVRName(VR vr)
  {
    auto iter = TheVRMap.find(vr);
    if (iter != TheVRMap.end())
    {
      return iter->second.first;
    }
    
    return std::string();
  }

  /*!
  Converts e.g. VR_US to "US"
  */
  std::string GetVRShortName(VR vr)
  {
    auto iter = TheVRMap.find(vr);
    if (iter != TheVRMap.end())
    {
      return iter->second.second;
    }
    
    return std::string();
  }

  /*!
  Converts e.g. "US" to VR_US
  */
  VR GetVRFromShortName(std::string strVR)
  {
    std::uint16_t v = 0x0000;
    v |= (strVR[1] << 8);
    v |= (strVR[0]);

    return VR(v);
  }

  std::ostream& operator << (std::ostream& out, VR vr)
  {
    out << GetVRShortName(vr);
    return out;
  }
}//namespace dicom
