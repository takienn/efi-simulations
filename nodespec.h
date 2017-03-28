#ifndef NODESPEC_H
#define NODESPEC_H

#include "ns3/core-module.h"
#include <ostream>

namespace ns3 {

class NodeSpec
{
public:
  enum NodeType
  {
    AP = 0,
    RELAY = 1,
    STA = 2,
    STA_NORMAL = 3
  };

  NodeSpec ();
  NodeSpec (uint32_t id, NodeType type, Vector2D position, uint32_t relayId, double oPsr = 1, double rPsr = 1, double nPsr = 1, double resourceRate = 0);
  NodeSpec (uint32_t id, NodeType type, Vector3D position, uint32_t relayId, double oPsr = 1, double rPsr = 1, double nPsr = 1, double resourceRate = 0);
  virtual ~NodeSpec ();

  void SetId (uint32_t id);
  uint32_t GetId (void);
  void SetType (NodeType type);
  NodeType GetType (void);
  void SetPsr (double psr, uint8_t psrType = 0); // Old Psr = 0, relay Par = 1, New Psr = 1
  double GetPsr (uint8_t psrType = 0);
  void SetPosition (Vector3D position);
  Vector3D GetPosition (void);
  void SetRelayId(uint32_t id);
  uint32_t GetRelayId (void);
  void SetResourceRate (double rate);
  double GetResourceRate (void);

  void Print (std::ostream &os) const;

private:
  uint32_t m_id;
  NodeType m_type;
  double m_oPsr;
  double m_rPsr;
  double m_nPsr;
  Vector3D m_position;
  uint32_t m_relayId;
  double m_resourceRate;
};

std::ostream& operator<< (std::ostream& os, NodeSpec const& nodeSpec);

}

#endif // NODESPEC_H
