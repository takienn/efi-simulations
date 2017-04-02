#ifndef EFI_EXPERIMENT_H
#define EFI_EXPERIMENT_H

#include "ns3/core-module.h"
#include "ns3/wifi-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "nodespec.h"
#include "psr-error-model.h"

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
  void Destroy (void);

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

  ApplicationContainer InstallApplications (NetDeviceContainer src, NetDeviceContainer dst);
  ApplicationContainer InstallApplications (NetDeviceContainer src, Ipv4Address address);
  ApplicationContainer InstallApplications (uint32_t relayId);

  NodeContainer GetNodes (NodeSpec::NodeType type) const;
  NetDeviceContainer GetNetDevices (NodeSpec::NodeType type) const;

  void ClusterWakeup(uint32_t id, Time time);
  void ClusterSleep(uint32_t id, Time time);

  void Run(bool downlink = true, double totResources = 100);
  void RunEfi(bool downlink = true, double totResources = 100);
  void RunNormal(bool downlink = true, double totResources = 100);

  void ResetStats();

  void UpdatePhyTxBytes(Ptr<NetDevice>, double value);
  void UpdatePhyRxBytes(Ptr<NetDevice>, double value);

  void UpdateQueueWait(Ptr<NetDevice> device, Time time);
  void UpdateQueueDrop(Ptr<NetDevice> device);
  void UpdateQueueEnqueue(Ptr<NetDevice> device);
  void UpdateRequeue(Ptr<NetDevice> device);


private:
  void Initialize ();
  void SetupEfiNode(Ptr<Node> node, NodeSpec::NodeType type, double psr, uint32_t relayId, double resRate = 100);
  void SetupNormalNode(Ptr<Node>node, NodeSpec::NodeType type, double psr, uint32_t relayId, double resRate = 100);

  void SetupReceivePacket (Ptr<NetDevice> device);
  void SetupReceivePacket (NetDeviceContainer devices);

  ApplicationContainer SetupReceivePacket (NodeContainer nodes);
  void ReceivePacket (Ptr<Socket> socket);

  bool ClientsAssociated(uint32_t id);

  void LogAssoc(Mac48Address sta, Mac48Address addr);
  void LogDeAssoc(Mac48Address sta, Mac48Address addr);

  void LogBusy (Time start, Time duration, WifiPhy::State state);

//  void MonitorPhyTx(Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu);
  void SetupQueueMonitoring(Ptr<NetDevice> device);
  void EnableActiveProbing(Ptr<StaWifiMac> mac);
  void SetupHooks (NetDeviceContainer devices);

  void SetupPsr(NetDeviceContainer devices, double val);
  Ptr<SpectrumChannel> m_channel; // SpectrumChannel used across the whole simulation
  NetDeviceContainer m_apDevice;
  std::map<uint32_t, NetDeviceContainer> m_clusterDevices;
  NetDeviceContainer m_relayClusterDevice;
  NetDeviceContainer m_relayToApDevice;


  std::map<Ptr<NetDevice>, uint64_t> m_packetsTotal;
  std::map<Ptr<NetDevice>, uint64_t> m_totalPhyTxBytes;
  std::map<Ptr<NetDevice>, uint64_t> m_totalPhyRxBytes;

  std::map<Ptr<NetDevice> , uint64_t> m_queueEnqueueRecord;
  std::map<Ptr<NetDevice> , uint64_t> m_queueRequeueRecord;
  std::map<Ptr<NetDevice>, std::pair<Time, uint64_t> > m_queueWaitRecord;
  std::map<Ptr<NetDevice>, uint64_t> m_queueDropRecord;

  std::map<uint32_t, double> m_nodePsrValues;


  std::map<uint32_t, Ipv4AddressHelper> m_relayApIpAddress;
  std::map<uint32_t, Ipv4AddressHelper> m_clusterIpAddress;

  std::map<uint32_t, double> m_relayResourceMap;

  std::map<uint32_t, uint32_t> m_relayAssocTable;

  ApplicationContainer m_clusterRecvApps;


  Ptr<UniformRandomVariable> m_rand;
  Ptr<UniformRandomVariable> m_rand2;

  bool m_isEfi;
//  std::map<Mac48Address, Ptr<WifiNetDevice> > m_deviceAddressMap;
};

}

#endif //EFI_EXPERIMENT_H

