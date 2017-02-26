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
  void SetPsr (double psr);
  double GetPsr (void);
  void SetPosition (Vector3D position);
  Vector3D GetPosition (void);

private:
  NodeType m_type;
  double m_psr;
  Vector3D m_position;
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

class Experiment
{
public:
  Experiment ();
  virtual ~Experiment ();
  void Initialize ();
  NetDeviceContainer CreateCluster (std::vector<NodeSpec> clusterNodes);
  NetDeviceContainer CreateMasterAp(NodeSpec apNodeSpec);
  NetDeviceContainer ConnectRelayToAp();

private:
  Ptr<SpectrumChannel> m_channel;
  WifiHelper m_wifiHelper;
  SpectrumWifiPhyHelper m_wifiPhyHelper;
  WifiMacHelper m_mac;
  NetDeviceContainer m_apDevice;
  NetDeviceContainer m_clusterDevices;
  NetDeviceContainer m_relayDevice;

};
}
#endif /* SCRATCH_WIFI_SURVIVE_H_ */
