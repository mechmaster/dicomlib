#pragma once

#include <string>

#include "DataSet.hpp"
#include "TransferSyntax.hpp"
#include "pdata.hpp"
#include "aaac.hpp"
#include "aarj.hpp"
#include "UIDs.hpp"

namespace dicom
{
  //!Thrown if connection is aborted.
  struct AssociationAborted : public dicom::exception
  {
    primitive::AAbortRQ m_abort_request;

    AssociationAborted(const primitive::AAbortRQ& abort_request) :
      dicom::exception("Association Aborted"),
      m_abort_request(abort_request)
    {
    }
  };

  //!Holds shared functionality for dicom client and server classes.
  /*!
  Manages reading and writing control messages and datasets to/from
  a socket.

  Keeps track of conditions under which association was set up
  */
  struct ServiceBase: boost::noncopyable
  {
    ServiceBase();
    virtual ~ServiceBase();

    void write(MessageControlHeader::Code msgHead, const DataSet& ds, const UID& AbstractSyntaxUID, TS ts);
    void write(Buffer& buffer, MessageControlHeader::Code msgHead, std::uint8_t PresentationContextID,
      std::uint32_t MaxPDULength);

    bool read(DataSet& command_or_data);

    void writeCommand(const DataSet& ds, const UID& uid);
    void writeDataSet(const DataSet& ds, const UID& uid);

    //I comment out the default TS because this should be determined by PresID on the fly. -Sam
    void readDynamic(/*Network::Socket& socket,*/Buffer& p_data_tf_buffer, MessageControlHeader::Code& msgHead,
      bool& ready_to_parse);

    void parseRawVRIntoDataSet(Buffer& p_data_tf_buffer, const MessageControlHeader::Code& msgHead, DataSet& command_or_data);

    std::uint8_t getPresentationContextID(const UID& uid);
    std::uint8_t getPresentationContextID(const UID& AbsUID, const UID& TrnUID);

    UID getTransferSyntaxUID(std::uint8_t PresentationContextID);

    //Following two parameters keep a record of the conditions under which
    //this services association was set up.

    //!The association we accepted.
    primitive::AAssociateRQ m_aassociateRQ;

    //!The presentation contexts we accepted.
    std::vector<primitive::PresentationContextAccept> m_acceptedPresentationContexts;

    //!The current PresentationContextID we receive in the latest PDV
    /*
    This member does not belong to this place. It should belong  PDV. However, the whole
    data structure has been messed up. It is too hard to correct it. Let's put it here
    for now. -Sam Shen Jan 22, 2007
    */
    std::uint8_t m_currentPresentationContextID;

    //this function should only be called on the client side because client decides which 
    //transfer syntax to use. -Sam
    void setCurrentPCID(std::uint8_t pcid)
    {
      m_currentPresentationContextID = pcid;
    }
  };
}//namespace dicom
