#pragma once

#include <vector>

#include "aarq.hpp"
#include "TransferSyntax.hpp"
#include "Utility.hpp"

namespace dicom
{
  //!Takes some of the grunt work out of maintaining a list of presentation contexts.
  /*!
  Please refer to DICOM Standard Part 8, Fig 9-1, also Table 9-11, 9-13, 9-15.

  Basically, Presentation Contexts are a series of Abstract Syntax(SOPClassUIDs), each 
  uid associated with a series of transfersyntax uids. The requirement is relative loose 
  in practice. 

  For example, in dcmtk3.5.4(CTN), storescu split the transfersyntax into two groups, 
  each group associated with on one PresentationContext ID, also meaning each SOPClassUID 
  presents in the Proposed Presentation Contexts twice. First with ExplicitVR_LittleEndian 
  only, the other with compression transfer syntax and ImplicitLIttleEndian. I am guessing 
  that the designer of dcmtk loves to use ExVR_LittleEndian. Since the CStoreSCP has to 
  reply for each Presentation Context ID, ExVR_LittleEndian has a bigger chance to be 
  accepted, not like the others in a group of transfer syntax.

  At the end, it is up to the CStoreSCP to decide which Presentation ID to use. I am 
  making the default as "First Come, First Serve", detail in this:
  1. When CStoreSCU propose the transfer syntax, it puts the preferred transfer syntax
  at the beginning of the list. But ImplicitVRLittleEndian must be one in the list.
  2. When CStoreSCP replies to the proposal, it should take the first acceptable one.

  - Sam Shen , Jan 26, 2007
  */
  class PresentationContexts : public  std::vector<primitive::PresentationContext>
  {
    OddSeries<std::uint8_t> m_IDGenerator;

  public:

    void Add(const dicom::UID& uid);
    void Add(const UID& uid, const TS ts);
    void Add(const std::vector<UID>& Abstract_Syntax_UIDs, const std::vector<UID>& Transfer_Syntaxs);
  };
}//namespace dicom
