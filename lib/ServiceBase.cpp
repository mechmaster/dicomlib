/************************************************************************
* DICOMLIB
* Copyright 2003 Sunnybrook and Women's College Health Science Center
* Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
* See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include <vector>
#include <iostream>
#include <stdexcept>

#include "Encoder.hpp"
#include "Decoder.hpp"
#include "aarj.hpp"
#include "aarq.hpp"
#include "iso646.h"

#include "ServiceBase.hpp"

namespace dicom
{
  using namespace primitive;

  ServiceBase::ServiceBase() :
    m_currentPresentationContextID(0) //0 is not a valid number for Presentation Context ID -Sam
  {
  }

  ServiceBase::~ServiceBase()
  {
  }

  void ServiceBase::write(MessageControlHeader::Code msgHead, const DataSet& ds, const UID& AbstractSyntaxUID, TS ts)
  {
    UID tsUID(ts.getUID());

    m_currentPresentationContextID = getPresentationContextID(AbstractSyntaxUID);

    int ByteOrder = ts.isBigEndian() ? __BIG_ENDIAN : __LITTLE_ENDIAN;
    Buffer buffer(ByteOrder);

    dicom::WriteToBuffer(ds, buffer, ts);

    std::uint8_t MaxPDULength = m_aassociateRQ.m_userInfo.m_maxSubLength.m_maximumLength;
    write(buffer, msgHead, m_currentPresentationContextID, MaxPDULength);
  }

  void ServiceBase::writeCommand(const DataSet& ds, const UID& uid)
  {
    //Commands MUST have VR/LE Transfer Syntax.
    write(MessageControlHeader::COMMAND, ds, uid, TS(IMPL_VR_LE_TRANSFER_SYNTAX));
  }

  void ServiceBase::writeDataSet(const dicom::DataSet& ds, const UID& uid)
  {
    TS ts = getTransferSyntaxUID(m_currentPresentationContextID);
    write(MessageControlHeader::DATASET, ds, uid, ts);
  }

  /*!
  This functionality used to be in PDATATF
  */
  void ServiceBase::write(Buffer& buffer, MessageControlHeader::Code msgHead,
    std::uint8_t PresentationContextID, std::uint32_t MaxPDULength)
  {
    if(buffer.position() != buffer.begin())
    {
      throw exception("buffer.position() != buffer.begin(), in ServiceBase::Write()");
    }

    //Does this ever happen?  Is this valid to do?  Must have
    //rationale here please.   TODO.
    if(MaxPDULength == 0)
    {
      MaxPDULength = static_cast<std::uint32_t>(buffer.size());
    }

    /*Network::Socket* socket = getSocket();

    while(buffer.position()!=buffer.end())
    {
      See Part 8, table 9-22 and 9-23 to understand what we're sending here,
      and realise that I've opted to only ever send ONE pdv with each P-DATA-TF.
      *socket << BYTE(0x04);
      *socket << BYTE(0x00);

      UINT32 BytesLeftToSend=static_cast<UINT32>((buffer.end() - buffer.position()));

      const UINT32 BytesInThisChunk=std::min<UINT32>(BytesLeftToSend, MaxPDULength - 6);

      *socket << UINT32(BytesInThisChunk + 6);
      //now the PDV item
      //We should use the PDV class to mediate this!

      *socket << UINT32(BytesInThisChunk + 2);
      *socket << PresentationContextID;

      if(buffer.position() + (BytesInThisChunk) == buffer.end())
      {
        msgHead |= MessageControlHeader::LAST_FRAGMENT;
      }

      *socket << msgHead;

      //then send data...
      BYTE* Begin= &(*(buffer.position()));
      socket->Sendn(Begin, (BytesInThisChunk));

      //buffer.position()+=(BytesInThisChunk);//this is now a bug...
      buffer.increment(BytesInThisChunk);
    }*/
  }

  /*!
  returns false on association termination, else true.

  (We really shouldn't _handle_ the termination here, functions should do
  one and only one thing.

  This needs to be re-thought - it's really ugly...
  */

  /*
  This function needs re-work. When servicebase,i.e. pdu read a dataset, it does 
  not know ahead what transfersyntax it will be. It is not defined in the command
  dataset. It is defined in the PresID in p_data_tf header. In principle(e.g. dcmtk), 
  one AbstractSyntax(SOPClass) can support multiple TS. In other words, we will 
  have to determine the ts on-the-fly. Or you can only support one ts, i.e. 
  implicitVR_littleEndian. That is why each time I try to support ExpliciVR
  I have a problem. 

  I change the interface of this function, removing the ts parameter. In theory,
  a p_data_tf can contain more than one dataset due to the multiplicity of pdv items.
  But in reality, because 1. the pdu's MaxSubSize is merely 16384, 2. nobody is doing that
  p_data_tf contains only one dataset, either a command or a dataset
  -Sam Shen Jan 29, 2007
  */
  bool ServiceBase::read(DataSet& command_or_data)
  {
    //Specified at Part 8/figure 9-2
    /*
    What we basically do here is pull data off of the TCP/IP stream onto
    a PDataTF object(which has a BUFFER), and then at the end read
    from the buffer onto the DataSet.  I'm not entirely happy about this
    design, but there doesn't seem to be any other feasible approach.
    */

    //Network::Socket* socket = getSocket();
    Buffer p_data_tf_buffer;
    MessageControlHeader::Code msgHead;

    while (true) //loop, apparently implying that we can expect more than one PDATATF object.
    {
      std::uint8_t ItemType = 0x00;
      //*socket >> ItemType;

      switch (ItemType)//shouldn't be 1, 2 or 3.
      {
        case 0x04: // P-DATA-TF
        {
          bool ready_to_parse = false;
          //readDynamic(*socket, p_data_tf_buffer, msgHead, ready_to_parse);
          if (ready_to_parse)//what is the corresponding 'else' ?
          {
            parseRawVRIntoDataSet(p_data_tf_buffer, msgHead, command_or_data);
            return true;
          }
          else
          {
            break; //keep going through loop
          }
        }
        break;
        case 0x05: // A-RELEASE-RQ
        {
          AReleaseRQ release_request;
          //release_request.readDynamic(*socket);

          // also drop
          AReleaseRP release_response;
          //release_response.write(*socket);
          return false;
        }
        case 0x07: // A-ABORT-RQ
        {
          //shouldn't we try to read the abort request here
          //AAbortRQ abort_request(*socket);
          //throw AssociationAborted(abort_request);
        }
        default:
        {
          AAbortRQ abort_request(AAbortRQ::DICOM_SERVICE_PROVIDER, AAbortRQ::UNRECOGNIZED_PDU);
          //abort_request.write(*socket);
          throw BadItemType(ItemType, 0);
        }
      }
    }
  }

  /*
  This function reads the socket and strip off the pdu/pdv fields and put the dataset (command
  or data)into a buffer for later parsing. It is from the previous class PDataTF -Sam
  */
  void ServiceBase::readDynamic(/*Network::Socket& socket,*/Buffer& p_data_tf_buffer, MessageControlHeader::Code& msgHead,
    bool& ready_to_parse)
  {
    /*UINT32 Count;
    //1. Read in the pdu fields
    //BYTE pdu_type has been read before entering this function
    BYTE pdu_reserve;
    socket >> pdu_reserve;
    UINT32 pdu_length;
    socket >> pdu_length;

    Count = pdu_length;
    while ( Count > 0)
    {
    UINT32 pdv_item_length;
    socket >> pdv_item_length;
    socket >> CurrentPresentationContextID_;
    socket >> msgHead;

    p_data_tf_buffer.insert(p_data_tf_buffer.end(),pdv_item_length-2,0x00);
    socket.Readn(&*(p_data_tf_buffer.end()-(pdv_item_length-2)),(pdv_item_length-2));//slightly more complicated.*/

    /*
    The previous 3 lines could be dramatically speeded up if
    we _first_ allocate the space on buffer, then pass _pointers_
    to the begin and end of the new space to Socket::Readn();

    I'm not going to change this till I have a good unit test for the change

    Meyers says in 'Efficient STL' that member functions are preferable
    over algorithms for efficiency?

    So possible optimizations would be:
    1)

    buffer_.insert(buffer_.end(),data.begin(),data.end());//this is simple.

    2)

    buffer_.insert(buffer_.end(),pdv.Length-2,0x00);
    socket.Readn(buffer_.end()-(pdv.Length-2),(pdv.Length-2));//slightly more complicated.

    We need profiling tests for both, such as dicomtest::SubmitLotsOfImages()
    */

    /*Count = Count - pdv_item_length - sizeof(UINT32);
    //Length = Length - pdv_item_length - sizeof(UINT32);

    if((msgHead bitand MessageControlHeader::LAST_FRAGMENT)!= 0)
    {
    ready_to_parse = true;
    return;
    }
    }

    return;*/
  }

  void ServiceBase::parseRawVRIntoDataSet(Buffer& p_data_tf_buffer, const MessageControlHeader::Code& msgHead,
    DataSet& command_or_data)
  {
    //first thing: determine the endian of the buffer

    if(p_data_tf_buffer.position() != p_data_tf_buffer.begin())
    {
      throw exception("buffer.position() != buffer.begin(), in ServiceBase::ParseRawVRIntoDataSet()");
    }

    //remember: command dataset is always little endian
    UID tsuid;
    if(!(msgHead & MessageControlHeader::COMMAND))
    {
      tsuid = getTransferSyntaxUID(m_currentPresentationContextID);
    }
    else
    {
      tsuid = IMPL_VR_LE_TRANSFER_SYNTAX;
    }

    ReadFromBuffer(p_data_tf_buffer, command_or_data, TS(tsuid));//defined in decoder.cpp
  }

  std::uint8_t ServiceBase::getPresentationContextID(const UID& uid)
  {
    //shouldn't we just be interested in accepted presentation contexts?-Trevor
    //But AcceptedPresentationContexts_ does not contain UID. -Sam
    const std::vector<PresentationContext>& PCArray = m_aassociateRQ.m_proposedPresentationContexts;

    size_t Index = 0;
    while (Index < PCArray.size())
    {
      const PresentationContext& PresContext = PCArray.at(Index);
      const PresentationContextAccept& APresContext = m_acceptedPresentationContexts.at(Index);
      if (PresContext.m_AbsSyntax.m_UID == uid && APresContext.m_result == 0)//The first accepted PresID ever found -Sam
      {
        return (PresContext.m_ID);
      }

      ++Index;
    }

    // we could replace above with a call to find_if ???

    // You're probably trying to use a SOP class that
    // wasn't negotiated during association!
    throw dicom::exception("Couldn't get Presentation Context ID");
  }

  /*!
  Get the PCID for a given AbsUID and TrnUID
  */

  std::uint8_t ServiceBase::getPresentationContextID(const UID& AbsUID, const UID& TrnUID)
  {
    //shouldn't we just be interested in accepted presentation contexts?

    for (auto& presContext : m_aassociateRQ.m_proposedPresentationContexts)
    {
      if (presContext.m_AbsSyntax.m_UID == AbsUID)
      {
        for (auto& item : m_acceptedPresentationContexts)
        {
          if (item.m_trnSyntax.m_UID == TrnUID && item.m_presentationContextID == presContext.m_ID)
          {
            return item.m_presentationContextID;
          }
        }
      }
    }

    throw dicom::exception("given presentation context does not exist with specified transfer syntax.");
  }

  UID ServiceBase::getTransferSyntaxUID(std::uint8_t PresentationContextID)
  {
    for (auto& item : m_acceptedPresentationContexts)
    {
      if (item.m_presentationContextID == PresentationContextID)
      {
        return item.m_trnSyntax.m_UID;
      }
    }

    throw std::runtime_error("Couldn't identify Presentation Context");
  }
}//namespace dicom
