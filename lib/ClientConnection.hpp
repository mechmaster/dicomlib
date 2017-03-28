#pragma once

#include <string>
#include <vector>

#include <boost/scoped_ptr.hpp>

#include "ServiceBase.hpp"
#include "aaac.hpp"
#include "PresentationContexts.hpp"
#include "QueryRetrieve.hpp"

namespace dicom
{
/*
This object is basically an SCU - Service Class User,
but that name seems to be already taken?

We need to be a bit careful here.  DICOM seems to think that every
entity can be both a service user and provider, and provides a negotiation
mechanism for specifying which SCP/SCU roles one will fulfill.  I think
my client class will only ever do SCU roles, and my server only SCP
*/

/*
When creating a client connection, the user needs to specify:

remote host
remote port

local and remote AETs

Presentation Contexts:  (See Part 7, Figure D.3-1)
{
PresentationContextID
Abstract Syntax Name (Service Object Pair Class UIDs or Meta-SOP class UIDs)
Transfer Syntax(s)

}

maximum length (probably handled by framework, can be different in each direction!)
Implementation details (name and UID.  UCDMC99 hard codes this, which is WRONG)
*/
/*
So with all this in mind, the constructor might look something like:

ClientConnection c(Host,Port,LocalAET,RemoteAET,ProposedAbstractSyntaxes);

Note that as usual we are using the RAII (Resource Aquisition Is Initialisation)
paradigm here.

(See Stroustup Ch 14)
*/

/*
I think our member function naming convention isn't ideal.
Store should be CStore, etc.
*/

  class ClientConnection : public ServiceBase
  {
    boost::scoped_ptr<Network::ClientSocket> m_socket;

  public:

    virtual Network::Socket* GetSocket();

    ClientConnection(std::string Host, unsigned short Port, std::string LocalAET, std::string RemoteAET,
      const PresentationContexts& ProposedPresentationContexts);
    virtual ~ClientConnection();

    //!Send a dataset, return response  (i.e. perform a C-STORE)
    DataSet store(const DataSet& query);//be careful with this, ts should match the syntax you negotiated.

    //!Provide a query, get back a list of matches
    std::vector<DataSet> find(const DataSet& query, QueryRetrieve::Root root);

    DataSet move(const std::string& destination, const DataSet& query, QueryRetrieve::Root root);
    DataSet echo();

  protected:

    bool interogateAAssociateAC(primitive::AAssociateAC& acknowledgement);
};
}//namespace dicom
