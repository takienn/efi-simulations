#ifndef EFI_EXPERIMENT_H
#define EFI_EXPERIMENT_H

#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "nodespec.h"

#include <map>

namespace ns3 {

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
  void InstallApplications (NetDeviceContainer src, Ipv4Address address);

  std::map<Ptr<NetDevice>, uint64_t> GetPacketsTotal ();

  NodeContainer GetNodes (NodeSpec::NodeType type) const;
  NetDeviceContainer GetNetDevices (NodeSpec::NodeType type) const;

  void Run(int argc, char *argv[]);

private:
  void Initialize ();
  void SetupNode(Ptr<Node>, NodeSpec::NodeType, double psr, uint32_t relayId, bool efiActive = true);
  void SetupReceivePacket (Ptr<NetDevice> device);
  void ReceivePacket (Ptr<Socket> socket);

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

}

#endif //EFI_EXPERIMENT_H

