/************************************************************************
*	DICOMLIB
*	Copyright 2003 Sunnybrook and Women's College Health Science Center
*	Implemented by Trevor Morgan  (morgan@sten.sunnybrook.utoronto.ca)
*
*	See LICENSE.txt for copyright and licensing info.
*************************************************************************/

#pragma once

#include <string>

#include <boost/function.hpp>

#include "DataSet.hpp"
#include "UIDs.hpp"

#include "ServiceBase.hpp"
#include "CommandSets.hpp"

namespace dicom
{

/*
First we define the signature of callback functions that will be called
by instances of ThreadSpecificServer on receiving CDIMSE commands, such as
C-MOVE, C-FIND etc...

A developer of a DICOM server must implement functions that match these
signatures and register them with a Server object by calling Server::AddHandler(...)

See comments in Server.hpp

We use boost::function to specify function signatures.  Note that due to limitations
in Microsoft Visual C++ 7.0, we need to provide two alternative syntaxes.

We should probably create a Callbacks.hpp file for these typedefs.
*/

#if defined(_MSC_VER) //MSVC v7.1 and above may not need this workaround.
  //msvc needs this non-standard syntax.  See the boost::function tutorial for reasoning.
  //see boost::function documentation for rationale of alternative syntax.
  typedef boost::function3<void, ServiceBase&, const DataSet&, DataSet&> HandlerFunction;
  typedef boost::function3<void, ServiceBase&, DataSet&, Sequence&> CFindFunction;
#else
  typedef boost::function<void(ServiceBase&, const DataSet& , DataSet&)> HandlerFunction;
  typedef boost::function<void(ServiceBase&, DataSet&, Sequence&)> CFindFunction;
#endif

  typedef HandlerFunction CMoveFunction;
  typedef HandlerFunction CStoreFunction;
  typedef HandlerFunction CGetFunction;

  void HandleCEcho(ServiceBase& pdu, const DataSet& command,const UID& classUID);

  void HandleCStore(CStoreFunction handler, ServiceBase& pdu, const DataSet& command, const UID& classUID);

  void HandleCFind(CFindFunction  handler,ServiceBase& pdu, const DataSet& command, const UID& classUID);

  void HandleCMove(CMoveFunction handler, ServiceBase& pdu, const DataSet& command, const UID& classUID);

  void HandleCGet(CGetFunction handler, ServiceBase& pdu, const DataSet& command, const UID& classUID);

  class CGetSCP
  {
    HandlerFunction m_handler;

  public:

    CGetSCP(HandlerFunction handler) :
      m_handler(handler)
    {
      
    }

    void handle(ServiceBase& pdu, const DataSet& rqCmd, const UID& classUID);
  };

  /*
  These classes should probably have a common base (that could
  containt the UID object.)
  */

  //!Service Class User.
  /*!
  Base class for the various Service Class Users
  */

  class SCU
  {

  protected:

    ServiceBase& m_service;
    const UID m_classUID;

  public:

    SCU(ServiceBase& service, UID classUID) :
      m_service(service),
      m_classUID(classUID)
    {
    }
  };


  //!Part 4, Annex A

  class CEchoSCU : public SCU
  {

  public:

    CEchoSCU(ServiceBase& service);//,const UID& classUID = VERIFICATION_SOP_CLASS);

    void writeRQ();
    void readRSP(std::uint16_t& stat_p);
    void readRSP(std::uint16_t& status, DataSet& response);
  };

  class CStoreSCU : public SCU
  {

  public:

    CStoreSCU(ServiceBase& service, const UID& classUID);

    void writeRQ(const UID& instUID, const DataSet& data, std::uint16_t priority = Priority::MEDIUM);
    void readRSP(std::uint16_t& status);
    void readRSP(std::uint16_t& status, DataSet& response);
  };

  class CFindSCU : public SCU
  {

  public:

    CFindSCU(ServiceBase& service, const UID& classUID);

    void writeRQ(const DataSet& data, std::uint16_t priority = Priority::MEDIUM);
    void readRSP(std::uint16_t& status, DataSet& data);
    void readRSP(std::uint16_t& status, DataSet& response, DataSet& data);
  };

  class CGetSCU : public SCU
  {

  public:

    CGetSCU(ServiceBase& service, const UID& classUID);

    void writeRQ(const DataSet& data, std::uint16_t priority = Priority::MEDIUM);
    void readRSP(std::uint16_t& status, DataSet& data);
    void readRSP(std::uint16_t& status, DataSet& response, DataSet& data);
  };

  class CMoveSCU : public SCU
  {

  public:

    CMoveSCU(ServiceBase& service, const UID& classUID);

    void writeRQ(const std::string& destAET, const DataSet& data, std::uint16_t priority = Priority::MEDIUM);
    void readRSP(std::uint16_t& status, DataSet& data);
    void readRSP(std::uint16_t& status, DataSet& response, DataSet& data);
  };
}//namespace dicom

