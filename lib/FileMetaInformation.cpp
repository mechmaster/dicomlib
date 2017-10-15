/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "FileMetaInformation.hpp"
#include "TransferSyntax.hpp"
#include "Encoder.hpp"
#include "Buffer.hpp"
#include "Decoder.hpp"
#include "Encoder.hpp"
#include "GroupLength.hpp"
#include "UIDs.hpp"

namespace dicom
{
  void VerifyAcceptable(Tag tag, UID uid)
  {
    //http://dicom.nema.org/Dicom/2013/output/chtml/part04/sect_B.5.html
    //http://dicom.nema.org/Dicom/2013/output/chtml/part10/chapter_7.html
  //verification code goes here.
    
    // Написать код проверки на собкласс по списку возможных значений.
  }


  /*!
  Refer to Part 10, Section 7.1 for a description of the fields here.
  */

  FileMetaInformation::FileMetaInformation(const DataSet& data, TS ts)
  {
    const UID& classUID = data(TAG_SOP_CLASS_UID).Get<UID>();
    const UID& instUID = data(TAG_SOP_INST_UID).Get<UID>();
    VerifyAcceptable(TAG_MEDIA_SOP_CLASS_UID,classUID);//table 7.1-1 of part 10 implies some checking should occur here...

    TypeFromVR<VR_OB>::Type VersionInfoData;//Set to (0,1) for the version of the standard that we support.
    VersionInfoData.push_back(0x00);
    VersionInfoData.push_back(0x01);
    m_metaElements.put<VR_OB>(TAG_FILE_INFO_VERS, VersionInfoData);

    m_metaElements.put<VR_UI>(TAG_MEDIA_SOP_CLASS_UID, classUID);
    m_metaElements.put<VR_UI>(TAG_MEDIA_SOP_INST_UID, instUID);
    m_metaElements.put<VR_UI>(TAG_TRANSFER_SYNTAX_UID, ts.getUID());
    m_metaElements.put<VR_UI>(TAG_IMPL_CLASS_UID, UID(ImplementationClassUID));
    m_metaElements.put<VR_SH>(TAG_IMPL_VERS_NAME, ImplementationVersionName);

    std::uint32_t group_length = GroupLength(m_metaElements, TS(EXPL_VR_LE_TRANSFER_SYNTAX));

    m_metaElements.put<VR_UL>(TAG_FILE_INFO_GR_LEN, group_length); //because multimap is sorted, this will
      //get inserted at the front of MetaElements_

    if(m_metaElements.find(TAG_FILE_INFO_GR_LEN) != m_metaElements.begin())
    {
      throw exception("TAG_FILE_INFO_GR_LEN not at begin."); //test above assertion!
    }
  }

  FileMetaInformation::FileMetaInformation(std::istream& In)
  {
    if (!In)
    {
      throw FileMetaInfoException("Input stream not open.");
    }

    if(!In.seekg(0))
    {
      throw FileMetaInfoException("Couldn't find beginning of stream.");
    }

    if(!In.read(reinterpret_cast<char*>(&m_preamble[0]), 128))
    {
      throw FileMetaInfoException("Couldn't read preamble.");
    }
    
    std::string prefix(4, 0);
    if(!In.read(&prefix[0], prefix.size()))
    {
      throw FileMetaInfoException("Couldn't read prefix.");
    }

    if (prefix != "DICM")
    {
      throw FileMetaInfoException("Prefix is not 'DICM'.");
    }

    Buffer buffer(__LITTLE_ENDIAN);//Section 7.1 says this has to be used.

    std::vector<std::uint8_t> temp(12, 0);
    In.read(reinterpret_cast<char*>(&temp[0]), temp.size());
    buffer.AddVector(temp);

    TS ts(EXPL_VR_LE_TRANSFER_SYNTAX); //Section 7.1 says this has to be used.

    ReadElementFromBuffer(buffer, m_metaElements, ts);//gets the 'length' element.

    Tag tag = m_metaElements.begin()->first;
    if(tag != TAG_FILE_INFO_GR_LEN)
    {
      throw exception("First tag must be group length(0x0002,0000) in File Meta Information");
    }

    Value& value = m_metaElements.begin()->second;

    std::uint32_t FileMetaInfoLength;
    value >> FileMetaInfoLength;

    //now read the rest of the file meta info from the input stream

    buffer.clear();

    temp.resize(FileMetaInfoLength);
    In.read(reinterpret_cast<char*>(&temp[0]), temp.size());
    buffer.AddVector(temp);

    //now parse onto meta info set.
    ReadFromBuffer(buffer, m_metaElements, ts);

    //done, and I should be at the correct point in the file to continue reading!
  }

  std::uint32_t FileMetaInformation::write(std::ostream& Out)
  {
    Out.seekp(0);//always go to the beginning

    Out.write(reinterpret_cast<char*>(&m_preamble[0]), m_preamble.size());

    std::string prefix("DICM");
    Out.write(prefix.c_str(), prefix.size());

    Buffer buffer(__LITTLE_ENDIAN);

    WriteToBuffer(m_metaElements, buffer, TS(EXPL_VR_LE_TRANSFER_SYNTAX));//Section 7.1 says this syntax has to be used.
    Out.write(reinterpret_cast<char*>(&buffer[0]), buffer.size());

    return (buffer.size() + 128 + 4);
  }
  
  DataSet FileMetaInformation::getMetaElements()
  {
    return m_metaElements;
  }
}//namespace dicom
