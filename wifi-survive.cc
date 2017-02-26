/*
 * wifi-survive.cc
 *
 *  Created on: Jul 23, 2016
 *      Author: Taqi Ad-Din
 */

#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/applications-module.h"
#include "ns3/network-module.h"
#include "ns3/propagation-module.h"
#include "ns3/config-store-module.h"
#include "ns3/log.h"

#include "wifi-survive.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("WifiSurvive");

NS_OBJECT_ENSURE_REGISTERED (PsrErrorRateModel);

TypeId PsrErrorRateModel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::PsrErrorRateModel")
    .SetParent<ErrorRateModel> ()
    .SetGroupName ("Wifi")
    .AddConstructor<PsrErrorRateModel> ()
    .AddAttribute("rate", "Error Rate", DoubleValue(0.0),
		  MakeDoubleAccessor(&PsrErrorRateModel::m_rate),
		  MakeDoubleChecker<double>(0.0, 1.0))
  ;
  return tid;
};

PsrErrorRateModel::PsrErrorRateModel ()
: m_rate(0.0)
{

};

PsrErrorRateModel::~PsrErrorRateModel ()
{

};

double PsrErrorRateModel::GetChunkSuccessRate (WifiMode mode, WifiTxVector txVector, double snr, uint32_t nbits) const
{
//    return m_ranvar->GetValue () < m_rate? 0.0:1.0;
  return (1 - m_rate); //TODO too simple, maybe needs some testing
};


double PsrErrorRateModel::GetRate (void) const
{
  return m_rate;
};

void PsrErrorRateModel::SetRate (double rate)
{
  m_rate = rate;
};


void PsrErrorRateModel::SetRandomVariable (Ptr<RandomVariableStream> ranvar)
{
  m_ranvar = ranvar;
};



NodeSpec::NodeSpec ()
  :m_type(STA), m_psr(1)
{

}

NodeSpec::~NodeSpec()
{

}

void NodeSpec::SetType (NodeType type)
{
  m_type = type;
}

NodeSpec::NodeType NodeSpec::GetType (void)
{
  return m_type;
}

void NodeSpec::SetPsr (double psr)
{
  m_psr = psr;
}

double NodeSpec::GetPsr (void)
{
  return m_psr;
}

void NodeSpec::SetPosition (Vector3D position)
{
  m_position = position;
}

Vector3D NodeSpec::GetPosition (void)
{
  return m_position;
}


Ptr<WifiPhy>
GetWifiPhyPtr (const NetDeviceContainer &devices)
{
  Ptr<WifiNetDevice> device = devices.Get (0)->GetObject<WifiNetDevice> ();
  Ptr<WifiPhy> phy = device->GetPhy ();
  return phy;
}

void
PrintAttributesIfEnabled (bool enabled)
{
  if (enabled)
    {
      ConfigStore outputConfig;
      outputConfig.ConfigureAttributes ();
    }
}


Experiment::Experiment ()
{

}

Experiment::~Experiment()
{

}

//Generate a connected cluster based on the Nodes descriptions, cluster[0] is always the group owner
NetDeviceContainer
Experiment::CreateCluster (std::vector<NodeSpec> clusterNodes)
{
  NodeContainer cluster;

//  MobilityHelper mobility; // TODO Specify Mobility

  for(std::vector<NodeSpec>::iterator it = clusterNodes.begin(); it != clusterNodes.end(); it++)
    {
      Ptr<Node> node = Create<Node> ();
      Ptr<ConstantPositionMobilityModel> mobility = CreateObject<ConstantPositionMobilityModel> ();
      mobility->SetPosition(it->GetPosition());
      node->AggregateObject(mobility);
      cluster.Add(node);
    }

  //TODO Should the FragmentationThreshold be set here or not down for the client nodes?
  m_wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager", "FragmentationThreshold", UintegerValue(600)); //TODO The actual value should be confirmed


  // Setting a the Relay as an access point to it's clulser's stations.
  std::stringstream ss;
  Ssid ssid;
  ss << "RelayAP" << cluster.Get(0)->GetId();
  ssid = Ssid (ss.str());

  m_mac.SetType ("ns3::ApWifiMac",
                 "QosSupported", BooleanValue (true),
                 "Ssid", SsidValue (ssid),
                 "BeaconGeneration", BooleanValue (true));

  NetDeviceContainer relayDevice = m_wifiHelper.Install (m_wifiPhyHelper, m_mac, cluster.Get(0));
  m_clusterDevices.Add(relayDevice);

  m_mac.SetType ("ns3::StaWifiMac",
               "QosSupported", BooleanValue (true),
               "Ssid", SsidValue (ssid),
	       "BE_MaxAmpduSize", UintegerValue(0)); //Disable AMPDU (BE_MaxAmpduSize=0) to make sure Fragmentation Threshold is always used

  for (uint32_t i = 1; i < cluster.GetN(); i++)
    {

      m_wifiPhyHelper.SetErrorRateModel("ns3::PsrErrorRateModel", "rate", DoubleValue(1.0 - clusterNodes[i].GetPsr()));
      NetDeviceContainer staDevice = m_wifiHelper.Install (m_wifiPhyHelper, m_mac, cluster.Get(i));
      m_clusterDevices.Add(staDevice);


//      PointerValue ptr;
//      Ptr<EdcaTxopN> edca;
//      Ptr<WifiNetDevice> wifiDev = staDevice.Get(0);
//      Ptr<WifiMac> wifiMac = wifiDev->GetMac();
//      wifiMac->GetAttribute("BE_EdcaTxopN", ptr);
//      edca = ptr.Get<EdcaTxopN>();
//      Time txopLimit = MilliSeconds(10);
//      edca->SetTxopLimit(txopLimit);
    }

  return m_clusterDevices;
}

/**
 * Create the Master AP to which the Relays will connect,
 * to forward traffic from clusters clients
 */
NetDeviceContainer
Experiment::CreateMasterAp(NodeSpec apNodeSpec)
{

  Ptr<ConstantPositionMobilityModel> mobility = CreateObject<ConstantPositionMobilityModel> ();
  mobility->SetPosition(apNodeSpec.GetPosition());

  Ptr<Node> apNode = CreateObject<Node> ();
  apNode->AggregateObject(mobility);

  Ssid ssid = Ssid("MasterAP");
  m_mac.SetType ("ns3::ApWifiMac",
                 "QosSupported", BooleanValue (true),
                 "Ssid", SsidValue (ssid),
                 "BeaconGeneration", BooleanValue (true));

  m_apDevice = m_wifiHelper.Install(m_wifiPhyHelper, m_mac, apNode);
  return m_apDevice;
}

/**
 * Connect a cluster Group Owner (Relay) to AP
 * TODO: Note here that WifiPhy uses the default NistErrorRateModel,
 * This should be checked if it fits the scenario.
 */
NetDeviceContainer
Experiment::ConnectRelayToAp ()
{
  NS_ASSERT(m_clusterDevices.GetN());

  //The relay is always at index 0
  Ptr<Node> relayNode = m_clusterDevices.Get(0)->GetNode();
  Ssid ssid = Ssid("MasterAP");
  m_mac.SetType ("ns3::StaWifiMac",
                 "QosSupported", BooleanValue (true),
                 "Ssid", SsidValue (ssid));

  m_relayDevice = m_wifiHelper.Install(m_wifiPhyHelper, m_mac, relayNode);
  return m_relayDevice;
}

void Experiment::Initialize ()
{
  SpectrumChannelHelper channelHelper;
  channelHelper.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  channelHelper.AddPropagationLoss("ns3::FixedRssLossModel");
  channelHelper.SetChannel("ns3::MultiModelSpectrumChannel");
  m_channel = channelHelper.Create();

  m_wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
  m_wifiHelper.SetRemoteStationManager ("ns3::ConstantRateWifiManager");

  m_wifiPhyHelper = SpectrumWifiPhyHelper::Default ();
  m_wifiPhyHelper.Set ("RxGain", DoubleValue (0.0));
  m_wifiPhyHelper.Set ("RxNoiseFigure", DoubleValue (0.0));
  m_wifiPhyHelper.Set ("EnergyDetectionThreshold", DoubleValue (-110.0));
  m_wifiPhyHelper.Set ("CcaMode1Threshold", DoubleValue (-110.0));
  m_wifiPhyHelper.SetChannel (m_channel);

}

int main (int argc, char *argv[])
{
  CommandLine cmd;

  double psrTh = 0.8;

  cmd.AddValue("psrTh","PSR Threshold that defines the zones", psrTh);

  Experiment experiment;
  experiment.Initialize();

  std::vector<NodeSpec> clusterNodes;
  NodeSpec ns1;
  ns1.SetPosition(Vector3D(20,20,0));
  ns1.SetPsr(1);
  ns1.SetType(NodeSpec::RELAY);

  NodeSpec ns2;
  ns2.SetPosition(Vector3D(40,20,0));
  ns2.SetPsr(1);
  ns2.SetType(NodeSpec::STA);

  NodeSpec ns3;
  ns3.SetPosition(Vector3D(20,40,0));
  ns3.SetPsr(1);
  ns3.SetType(NodeSpec::STA);

  clusterNodes.push_back(ns1);
  clusterNodes.push_back(ns2);
  clusterNodes.push_back(ns3);

  NodeSpec ap;
  ap.SetPosition(Vector3D(0,0,0));
  ap.SetPsr(1); //TODO Should be ignored ?
  ns2.SetType(NodeSpec::AP);

  NetDeviceContainer apDevice = experiment.CreateMasterAp(ap);
  NetDeviceContainer clusterDevices = experiment.CreateCluster(clusterNodes);
  NetDeviceContainer relayDevice = experiment.ConnectRelayToAp();
}



