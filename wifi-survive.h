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
#include <map>

namespace ns3 {

class NodeSpec
{
public:
  NodeSpec ();
  virtual ~NodeSpec ();

  enum NodeType
  {
    AP = 0,
    RELAY = 1,
    STA = 2
  };

  void SetType (NodeType type);
  NodeType GetType (void);
  void SetPer (double Per);
  double GetPer (void);
  void SetPosition (Vector3D position);
  Vector3D GetPosition (void);
  void SetSsid (Ssid ssid);
  Ssid GetSsid (void);
private:
  NodeType m_type;
  double m_Per;
  Vector3D m_position;
  Ssid m_ssid;
};

/**
 * A model for the error rate based on manually set PER values,
 * related to simulating E-FI paper proposal.
 */
class PerErrorRateModel : public ErrorRateModel
{
public:
  static TypeId GetTypeId (void);
  PerErrorRateModel ();
  virtual ~PerErrorRateModel();
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
  void Initialize ();

  void CreateNodes (std::vector<NodeSpec> nodes);

  /*
   * Creates the relay (always at index 0) ,
   * and the clients connected to this relay (forming the cluster)
   * and setup up addresses and networking stacks
   */
  NetDeviceContainer CreateCluster ();

  /*
   * Creates the Master Access Point to which the Relay should connect
   * and setup up its addresses and networking stacks, and connect the relay
   */

  NetDeviceContainer CreateMasterAp();


  void ConnectStaToAp (NodeContainer stas, NodeContainer ap, Ssid ssid = Ssid ("MasterAP"), uint32_t channelNumber = 0);

  void ReceivePacket (Ptr <Socket> socket);
  void InstallApplications ();

  std::map<Ptr<NetDevice>, uint64_t> GetPacketsTotal ();

  NodeContainer GetNodes (NodeSpec::NodeType type) const;

  void FixRouting (NodeContainer nodes);
private:
  Ptr<SpectrumChannel> m_channel; // SpectrumChannel used across the whole simulation
  NetDeviceContainer m_apDevice;
  NetDeviceContainer m_clusterDevices;
  NetDeviceContainer m_relayClusterDevice;
  NetDeviceContainer m_relayToApDevice;

  NodeContainer m_apNode;
  NodeContainer m_relayNode;
  NodeContainer m_clusterNodes;

  std::map<Ptr<NetDevice>, uint64_t> m_packetsTotal;
  std::map<uint32_t, double> m_nodePerValues;



};
}
#endif /* SCRATCH_WIFI_SURVIVE_H_ */
