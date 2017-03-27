#ifndef EFI_TOPOLOGY_READER_H
#define EFI_TOPOLOGY_READER_H

#include "ns3/topology-reader.h"
#include "nodespec.h"
#include <ostream>
#include <sstream>
#include <vector>

namespace ns3 {

class EfiTopologyReader : public TopologyReader
{
public:
  static TypeId GetTypeId (void);
  EfiTopologyReader ();
  virtual ~EfiTopologyReader ();
  virtual NodeContainer Read (void);
  std::vector<std::vector<NodeSpec> > ReadNodeSpec (void);

private:
  EfiTopologyReader (const EfiTopologyReader&);
  EfiTopologyReader& operator= (const EfiTopologyReader&);

  std::vector<NodeSpec> ParseNodes(std::istringstream lineBuffer);
  void ParseResources(std::istringstream lineBuffer);

  std::vector<NodeSpec> m_nodeSpecs;
};

std::ostream& operator<< (std::ostream& os, NodeSpec const& nodeSpec);
}

#endif //EFI_TOPOLOGY_READER_H
