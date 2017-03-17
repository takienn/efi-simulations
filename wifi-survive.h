/*
 * wifi-survive.h
 *
 *  Created on: Feb 25, 2017
 *      Author: kentux
 */

#ifndef SCRATCH_WIFI_SURVIVE_H_
#define SCRATCH_WIFI_SURVIVE_H_

#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/topology-reader.h"
#include <map>

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


/**
 *
 * A model for the error rate based on manually set PER values,
 * related to simulating E-FI paper proposal.
 */
class PsrErrorRateModel : public ErrorRateModel
{
public:
  static TypeId GetTypeId (void);
  PsrErrorRateModel ();
  virtual ~PsrErrorRateModel();
  double GetChunkSuccessRate (WifiMode mode, WifiTxVector txVector, double snr, uint32_t nbits) const;

  /**
   * \returns the error rate being applied by the model
   */
  double GetRate (void) const;

  /**
   * \param rate the error rate to be used by the model
   */
  void SetRate (double rate);

  /**
   * \param ranvar A random variable distribution to generate random variables
   */
  void SetRandomVariable (Ptr<RandomVariableStream> ranvar);

private:
  double m_rate;
  Ptr<RandomVariableStream> m_ranvar;
};


/*
 * A class to organize basic steps used to create the simulation
 */
class Experiment
{
public:
  Experiment ();
  virtual ~Experiment ();

  void CreateNodes (std::vector<NodeSpec> nodes, bool efiActive = true);

//  /*
//   * Creates the relay (always at index 0) ,
//   * and the clients connected to this relay (forming the cluster)
//   * and setup up addresses and networking stacks
//   */
//  NetDeviceContainer CreateCluster ();
//
//  /*
//   * Creates the Master Access Point to which the Relay should connect
//   * and setup up its addresses and networking stacks, and connect the relay
//   */
//  NetDeviceContainer CreateMasterAp();
//  void ConnectStaToAp (NodeContainer stas, NodeContainer ap, Ssid ssid = Ssid ("MasterAP"), uint32_t channelNumber = 0);

  void InstallApplications (NetDeviceContainer src, NetDeviceContainer dst);

  std::map<Ptr<NetDevice>, uint64_t> GetPacketsTotal ();

  NodeContainer GetNodes (NodeSpec::NodeType type) const;
  NetDeviceContainer GetNetDevices (NodeSpec::NodeType type) const;

  void Run(int argc, char *argv[]);

private:
  void Initialize ();
  void SetupNode(Ptr<Node>, NodeSpec::NodeType, double psr, uint32_t relayId, bool efiActive = true);
  void SetupReceivePacket (Ptr<NetDevice> device);
  void ReceivePacket (Ptr <Socket> socket);

  Ptr<SpectrumChannel> m_channel; // SpectrumChannel used across the whole simulation
  NetDeviceContainer m_apDevice;
  std::map<uint32_t, NetDeviceContainer> m_clusterDevices;
  NetDeviceContainer m_relayClusterDevice;
  NetDeviceContainer m_relayToApDevice;

  NodeContainer m_allNodes;
  NodeContainer m_apNodes;
  std::map<uint32_t, NodeContainer> m_relayNodes;
  std::map<uint32_t, NodeContainer> m_clusterNodes;

  std::map<Ptr<NetDevice>, uint64_t> m_packetsTotal;
  std::map<uint32_t, double> m_nodePsrValues;


  std::map<uint32_t, Ipv4AddressHelper> m_relayApIpAddress;
  std::map<uint32_t, Ipv4AddressHelper> m_clusterIpAddress;


};

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
#endif /* SCRATCH_WIFI_SURVIVE_H_ */
