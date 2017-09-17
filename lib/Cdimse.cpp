/************************************************************************
* DICOMLIB
* Copyright 2003 Sunnybrook and Women's College Health Science Center
* Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
* See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#include "Cdimse.hpp"

/*
This file needs a lot of cleaning up work.
*/
/*
I'm a bit worried that the 'Group Length' field
never seems to get inserted, as the standard seems
to require for all messages.  Am I missing something
here?
*/

namespace dicom
{
/*!
Simply write back a success response.
See Part 8, table 9.1-5
*/

  void HandleCEcho(ServiceBase& pdu, const DataSet& command, const UID& classUID)
  {
    std::uint16_t msgID;
    command(TAG_MSG_ID ) >> msgID;

    CommandSet::CEchoRSP response(msgID, classUID);

    pdu.WriteCommand(response, classUID);
  }

  void HandleCStore(CStoreFunction handler, ServiceBase& pdu, const DataSet& command, const UID& classUID)
  {
    std::uint16_t msgID;
    std::uint16_t data_set_status;

    command(TAG_MSG_ID) >> msgID;
    command(TAG_DATA_SET_TYPE) >> data_set_status;

    if (data_set_status == DataSetStatus::NO_DATA_SET)
    {
      throw exception("No data set!");
    }

    DataSet data;
    pdu.Read(data);//the TransferSyntax is determined internally by pdu. -Sam

    handler(pdu, command, data);//this should indicate failure via a throw...

    UID instuid;
    data(TAG_SOP_INST_UID) >> instuid;

    CommandSet::CStoreRSP response(msgID, classUID, instuid, Status::SUCCESS);

    pdu.WriteCommand(response, classUID);
  }

  /*
  Part 7, Section 9.1.2.2 describes this procedure...
  also table 9.3-3
  Part 4, Section C.3.4 has additional information.
  */

  void HandleCFind(CFindFunction handler, ServiceBase& pdu, const DataSet& command, const UID& classUID)
  {
#ifdef _DEBUG
    std::cout  << "HandleCFind:" << std::endl << command;
#endif

    std::uint16_t msgID;
    std::uint16_t data_set_status;

    command(TAG_MSG_ID) >> msgID;
    command(TAG_DATA_SET_TYPE) >> data_set_status;

    if (data_set_status == DataSetStatus::NO_DATA_SET)
    {
      throw exception("No data set");
    }

    DataSet request_data;
    pdu.Read(request_data);

    Sequence Matches;

    //the user-defined callback does the actual matching...
    handler(pdu, request_data, Matches);

    //now we send back all found matches.
    for (Sequence::iterator I = Matches.begin(); I != Matches.end(); I++)
    {
      CommandSet::CFindRSP response(msgID, classUID, Status::PENDING, DataSetStatus::YES_DATA_SET);

      pdu.WriteCommand(response, classUID);
      pdu.WriteDataSet(*I, classUID);
    }

    CommandSet::CFindRSP response(msgID, classUID, Status::SUCCESS, DataSetStatus::NO_DATA_SET);

    pdu.WriteCommand(response, classUID);
  }

  /*
  C-GET is only maintained in
  the standard for backwards compatability.  If we're going to implement it, I
  think we need to figure out the client-side behaviour first.
  */

  void CGetSCP::handle(ServiceBase& pdu, const DataSet& rqCmd, const UID& classUID)
  {
    //TODO
    throw NotYetImplemented();
  }

  void HandleCMove(CMoveFunction handler, ServiceBase& pdu, const DataSet& command, const UID& classUID)
  {
    std::uint16_t data_set_status;

    command(TAG_DATA_SET_TYPE) >> data_set_status;

    if(data_set_status == DataSetStatus::NO_DATA_SET)
    {
      throw exception("No data set");
    }

    DataSet request_data;
    pdu.Read(request_data);

    //The rest part of implementation involves design of server and should be 
    //implemented in serve. -Sam
    handler(pdu, command, request_data);
  }

  CEchoSCU::CEchoSCU(ServiceBase& service) :
    SCU(service, VERIFICATION_SOP_CLASS)
  {
  }

  void CEchoSCU::writeRQ()
  {
    CommandSet::CEchoRQ rq(getMessageID(), m_classUID);
    m_service.WriteCommand(rq, m_classUID);
  }

  void CEchoSCU::readRSP(std::uint16_t& status)
  {
    DataSet response;
    readRSP(status, response);
  }

  void CEchoSCU::readRSP(std::uint16_t& status, DataSet& response)
  {
    m_service.Read(response);
    response(TAG_STATUS) >> status;
  }

  CStoreSCU::CStoreSCU(ServiceBase& service, const UID& classUID) :
    SCU(service, classUID)
  {
  }

  void CStoreSCU::writeRQ(const UID& instUID, const DataSet& data, std::uint16_t priority)
  {
    CommandSet::CStoreRQ rq(getMessageID(), m_classUID, instUID, priority);
    m_service.WriteCommand(rq, m_classUID);
    m_service.WriteDataSet(data, m_classUID);
  }

  void CStoreSCU::readRSP(std::uint16_t& status) //maybe status should be a return value?TODO
  {
    DataSet response;
    readRSP(status, response);
  }

  void CStoreSCU::readRSP(std::uint16_t& status, DataSet& response)
  {
    m_service.Read(response);
    response(TAG_STATUS) >> status;
  }
  //I'd prefer:
  /*
  DataSet CStoreSCU::readRSP();
  */

  CFindSCU::CFindSCU(ServiceBase& service,const UID& classUID) :
    SCU(service, classUID)
  {
  }

  void CFindSCU::writeRQ(const DataSet& data, std::uint16_t priority)
  {
    CommandSet::CFindRQ rq(getMessageID(), m_classUID, priority);
    m_service.WriteCommand(rq, m_classUID);
    m_service.WriteDataSet(data, m_classUID);
  }

  void CFindSCU::readRSP(std::uint16_t& status, DataSet& data)
  {
    DataSet response;
    readRSP(status, response, data);
  }


  //All the ::readRSP functions from here on are identical: please
  //amalgamate!

  void CFindSCU::readRSP(std::uint16_t& status, DataSet& response, DataSet& data)
  {
    std::uint16_t dstype = 0;

    m_service.Read(response);

    response(TAG_DATA_SET_TYPE) >> dstype;
    response(TAG_STATUS) >> status;

    if (dstype != DataSetStatus::NO_DATA_SET)
    {
      m_service.Read(data);
    }
  }

  CGetSCU::CGetSCU(ServiceBase& service,const UID& classUID) :
    SCU(service, classUID)
  {
  }

  void CGetSCU::writeRQ(const DataSet& data, std::uint16_t priority)
  {
    CommandSet::CGetRQ rq(getMessageID(), m_classUID, priority);
    m_service.WriteCommand(rq, m_classUID);
    m_service.WriteDataSet(data, m_classUID);
  }

  void CGetSCU::readRSP(std::uint16_t& status, DataSet&  data)
  {
    DataSet rsp;
    readRSP(status, rsp, data);
  }

  void CGetSCU::readRSP(std::uint16_t& status, DataSet& response, DataSet&  data)
  {
    std::uint16_t dstype = 0;

    m_service.Read(response);

    response(TAG_DATA_SET_TYPE) >> dstype;
    response(TAG_STATUS) >> status;

    if (dstype != DataSetStatus::NO_DATA_SET)
    {
      m_service.Read(data);
    }
  }

  CMoveSCU::CMoveSCU(ServiceBase& service,const UID& classUID) :
    SCU(service, classUID)
  {
  }

  void CMoveSCU::writeRQ(const std::string& destAET, const DataSet& data, std::uint16_t priority)
  {
    CommandSet::CMoveRQ rq(getMessageID(), m_classUID, destAET, priority);
    m_service.WriteCommand(rq, m_classUID);
    m_service.WriteDataSet(data, m_classUID);
  }

  /*
  Now I'm not happy about these extra readRSP members, one of them
  is superfluous
  */

  void CMoveSCU::readRSP(std::uint16_t& status, DataSet& data)
  {
    DataSet response;
    readRSP(status, response, data);
  }

  void CMoveSCU::readRSP(std::uint16_t& status, DataSet& response, DataSet&  data)
  {
    std::uint16_t dstype = 0;

    m_service.Read(response);

    response(TAG_DATA_SET_TYPE) >> dstype;
    response(TAG_STATUS) >> status;

    if (dstype != DataSetStatus::NO_DATA_SET)
    {
      m_service.Read(data);
    }
  }
}//namespace dicom
