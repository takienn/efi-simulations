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
#include "ns3/internet-module.h"
#include "ns3/propagation-module.h"
#include "ns3/config-store-module.h"
#include "ns3/olsr-module.h"
#include "ns3/log.h"
#include "wifi-survive.h"

#include <ostream>
#include <fstream>

using namespace ns3;

uint16_t port = 9;

NS_LOG_COMPONENT_DEFINE ("WifiSurvive");

NS_OBJECT_ENSURE_REGISTERED (PsrErrorRateModel);

TypeId
PsrErrorRateModel::GetTypeId (void)
{
  static TypeId tid =
      TypeId ("ns3::PsrErrorRateModel").SetParent<ErrorRateModel> ().SetGroupName (
	  "Wifi").AddConstructor<PsrErrorRateModel> ().AddAttribute (
	  "rate", "Error Rate", DoubleValue (0.0),
	  MakeDoubleAccessor (&PsrErrorRateModel::m_rate),
	  MakeDoubleChecker<double> (0.0, 1.0));
  return tid;
}

PsrErrorRateModel::PsrErrorRateModel ()
    : m_rate (0.0)
{

}

PsrErrorRateModel::~PsrErrorRateModel ()
{

}

double
PsrErrorRateModel::GetChunkSuccessRate (WifiMode mode, WifiTxVector txVector,
					double snr, uint32_t nbits) const
{
//    return m_ranvar->GetValue () < m_rate? 0.0:1.0;
  return (1 - m_rate); //TODO too simple, maybe needs some testing
}

double
PsrErrorRateModel::GetRate (void) const
{
  return m_rate;
}

void
PsrErrorRateModel::SetRate (double rate)
{
  m_rate = rate;
}

void
PsrErrorRateModel::SetRandomVariable (Ptr<RandomVariableStream> ranvar)
{
  m_ranvar = ranvar;
}


////////// Implementing NodeSpec class ////////////////

NodeSpec::NodeSpec ()
    : m_type (STA), m_psr (1)
{

}

NodeSpec::~NodeSpec ()
{

}

void
NodeSpec::SetType (NodeType type)
{
  m_type = type;
}

NodeSpec::NodeType
NodeSpec::GetType (void)
{
  return m_type;
}

void
NodeSpec::SetPsr (double psr)
{
  m_psr = psr;
}

double
NodeSpec::GetPsr (void)
{
  return m_psr;
}

void
NodeSpec::SetPosition (Vector3D position)
{
  m_position = position;
}

Vector3D NodeSpec::GetPosition (void)
{
  return m_position;
}

Experiment::Experiment ()
{

}

Experiment::~Experiment()
{

}

void
LogAssoc (Mac48Address addr)
{
  std::cout << "Associated with " << addr << std::endl;
}

void
Experiment::CreateNodes (std::vector<NodeSpec> nodeSpecs)
{
  for(uint32_t i = 0; i != nodeSpecs.size(); i++)
    {
      Ptr<Node> node = CreateObject<Node> ();
      Ptr<ConstantPositionMobilityModel> mobility = CreateObject<ConstantPositionMobilityModel> ();
      mobility->SetPosition(nodeSpecs[i].GetPosition());
      node->AggregateObject(mobility);
      m_nodePsrValues[node->GetId()] = nodeSpecs[i].GetPsr();

      if(nodeSpecs[i].GetType()==NodeSpec::RELAY)
	m_relayNode.Add(node);
      if(nodeSpecs[i].GetType()==NodeSpec::STA)
	m_clusterNodes.Add(node);
      if(nodeSpecs[i].GetType()==NodeSpec::AP)
	m_apNode.Add(node);
    }
}
//Generate a connected cluster based on the Nodes descriptions, cluster[0] is always the group owner
NetDeviceContainer
Experiment::CreateCluster ()
{

  WifiHelper wifiHelper;
  wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
  //TODO Should the FragmentationThreshold be set here or not down for the client nodes?
  wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager", "FragmentationThreshold", UintegerValue(600)); //TODO The actual value should be confirmed

  SpectrumWifiPhyHelper wifiPhyHelper;
  wifiPhyHelper = SpectrumWifiPhyHelper::Default ();
//  wifiPhyHelper.Set ("RxGain", DoubleValue (0.0));
//  wifiPhyHelper.Set ("RxNoiseFigure", DoubleValue (0.0));
//  wifiPhyHelper.Set ("EnergyDetectionThreshold", DoubleValue (-110.0));
//  wifiPhyHelper.Set ("CcaMode1Threshold", DoubleValue (-110.0));
  wifiPhyHelper.Set ("ChannelNumber", UintegerValue(0));
  wifiPhyHelper.SetChannel (m_channel);

  WifiMacHelper macHelper;

  // Setting a the Relay as an access point to it's clulser's stations.
  std::stringstream ss;
  Ssid ssid;
  ss << "RelayAP" << m_relayNode.Get(0)->GetId();
  ssid = Ssid (ss.str());

  macHelper.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));

  NetDeviceContainer relayDevice = wifiHelper.Install (wifiPhyHelper, macHelper, m_relayNode.Get(0));
  m_relayClusterDevice.Add(relayDevice);
  m_packetsTotal[relayDevice.Get(0)] = 0;

  macHelper.SetType ("ns3::StaWifiMac",
		 "Ssid", SsidValue (ssid),
		 "BE_MaxAmpduSize", UintegerValue(0)); //Disable AMPDU (BE_MaxAmpduSize=0) to make sure Fragmentation Threshold is always used

  for (uint32_t i =0; i < m_clusterNodes.GetN(); i++)
    {
      wifiPhyHelper.SetErrorRateModel("ns3::PsrErrorRateModel",
				      "rate", DoubleValue(1.0 - m_nodePsrValues[m_clusterNodes.Get(i)->GetId()]));
      NetDeviceContainer staDevice = wifiHelper.Install (wifiPhyHelper, macHelper, m_clusterNodes.Get(i));
      m_clusterDevices.Add(staDevice);
      staDevice.Get(0)->GetObject<WifiNetDevice>()->GetMac()->TraceConnectWithoutContext ("Assoc", MakeCallback(&LogAssoc));

      m_packetsTotal[staDevice.Get(0)] = 0;
    }

  InternetStackHelper internet;
  OlsrHelper olsr;
  Ipv4StaticRoutingHelper staticRouting;
  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (olsr, 10);
  internet.SetRoutingHelper (list);

  internet.Install(m_relayNode);
  internet.Install(m_clusterNodes);

  Ipv4AddressHelper addressHelper;
  addressHelper.SetBase(Ipv4Address("10.0.1.0"), Ipv4Mask("255.255.255.0"));
  Ipv4InterfaceContainer relayInterface = addressHelper.Assign (m_relayClusterDevice);
  Ipv4InterfaceContainer clusterInterfaces = addressHelper.Assign (m_clusterDevices);


  NetDeviceContainer allDevices = NetDeviceContainer (m_relayClusterDevice, m_clusterDevices);
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  for(uint32_t i = 0; i < allDevices.GetN(); i++)
    {
      Ptr<Ipv4L3Protocol> ipv4 = allDevices.Get(i)->GetNode()->GetObject<Ipv4L3Protocol> ();
      Ipv4InterfaceAddress addr = ipv4->GetInterface(ipv4->GetInterfaceForDevice(allDevices.Get(i)))->GetAddress(0);

      InetSocketAddress local = InetSocketAddress (addr.GetLocal(), port);

      Ptr <Socket> sink = Socket::CreateSocket (allDevices.Get(i)->GetNode(), tid);

      sink->Bind (local);
      sink->BindToNetDevice(allDevices.Get(i));
      sink->SetRecvCallback (MakeCallback ( &Experiment::ReceivePacket, this));
    }

  return m_clusterDevices;
}

/**
 * Create the Master AP to which the Relays will connect,
 * to forward traffic from clusters clients, and setup its
 * addressing and networking stacks
 */
NetDeviceContainer
Experiment::CreateMasterAp()
{

  WifiHelper wifiHelper;
  wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
//  TODO Should the FragmentationThreshold be set here or not down for the client nodes?
  wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager");//, "FragmentationThreshold", UintegerValue(600)); //TODO The actual value should be confirmed

  SpectrumWifiPhyHelper wifiPhyHelper;
  wifiPhyHelper = SpectrumWifiPhyHelper::Default ();
  wifiPhyHelper.SetChannel(m_channel);
  wifiPhyHelper.Set("ChannelNumber", UintegerValue(44));


  Ssid ssid = Ssid("MasterAP");
  WifiMacHelper macHelper;
  macHelper.SetType ("ns3::ApWifiMac",
                 "Ssid", SsidValue (ssid));
  m_apDevice = wifiHelper.Install(wifiPhyHelper, macHelper, m_apNode);
  m_packetsTotal[m_apDevice.Get(0)] = 0;

  macHelper.SetType ("ns3::StaWifiMac",
                 "Ssid", SsidValue (ssid));
  m_relayToApDevice = wifiHelper.Install(wifiPhyHelper, macHelper, m_relayNode);
  m_packetsTotal[m_relayToApDevice.Get(0)] = 0;

  m_relayToApDevice.Get(0)->GetObject<WifiNetDevice>()->GetMac()->TraceConnectWithoutContext ("Assoc", MakeCallback(&LogAssoc));

  InternetStackHelper internet;
  OlsrHelper olsr;
  Ipv4StaticRoutingHelper staticRouting;

  Ipv4ListRoutingHelper list;
  list.Add (staticRouting, 0);
  list.Add (olsr, 10);
  internet.SetRoutingHelper (list);

  internet.Install(m_apNode);
//  internet.Install(m_relayNode);

  Ipv4AddressHelper addressHelper;
  addressHelper.SetBase("10.2.2.0", "255.255.255.0");
  addressHelper.Assign (m_relayToApDevice);
  addressHelper.Assign (m_apDevice);

  NetDeviceContainer allDevices = NetDeviceContainer(m_relayToApDevice, m_apDevice);
  TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
  for(uint32_t i = 0; i < allDevices.GetN(); i++)
    {
      Ptr<Ipv4L3Protocol> ipv4 = allDevices.Get(i)->GetNode()->GetObject<Ipv4L3Protocol> ();
      Ipv4InterfaceAddress addr = ipv4->GetInterface(ipv4->GetInterfaceForDevice(allDevices.Get(i)))->GetAddress(0);

      InetSocketAddress local = InetSocketAddress (addr.GetLocal(), port);

      Ptr <Socket> sink = Socket::CreateSocket (allDevices.Get(i)->GetNode(), tid);

      sink->Bind (local);
      sink->BindToNetDevice(allDevices.Get(i));
      sink->SetRecvCallback (MakeCallback ( &Experiment::ReceivePacket, this));
    }
  return m_apDevice;
}

/**
 * Connect a cluster Group Owner (Relay) to AP
 * TODO: Note here that WifiPhy uses the default NistErrorRateModel,
 * This should be checked if it fits the scenario.
 */

void
Experiment::ReceivePacket (Ptr <Socket> socket)
{
//  NS_LOG_UNCOND (Simulator::Now ().GetSeconds () << " Received one packet!");
  Ptr <Packet> packet;
  Address addr;
  socket->GetSockName (addr);
  InetSocketAddress iaddr = InetSocketAddress::ConvertFrom (addr);
  Ptr<Ipv4L3Protocol> ipv4l3p = socket->GetNode()->GetObject<Ipv4L3Protocol>();
  Ptr<NetDevice> device = ipv4l3p->GetInterface(ipv4l3p->GetInterfaceForAddress(iaddr.GetIpv4()))->GetDevice();

  packet = socket->Recv();
  while (packet)
    {
      m_packetsTotal[device]++;
      packet = socket->Recv();
    }
}

void
Experiment::InstallApplications ()
{

  NodeContainer allNodes;
  allNodes.Add(m_apNode);
  allNodes.Add(m_clusterNodes);

//  m_internet.Install (allNodes);

//  relayToApInterfaces.Add(m_addressHelper.Assign(m_relayToApDevice));

//  m_addressHelper.SetBase ("10.1.2.0", "255.255.255.0");

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();
//  FixRouting (NodeContainer(m_clusterNodes.Get(0)));

  Ptr<Ipv4L3Protocol> ipv4 = m_relayNode.Get(0)->GetObject<Ipv4L3Protocol> ();
  Ipv4InterfaceAddress addr = ipv4->GetInterface(ipv4->GetInterfaceForDevice(m_relayClusterDevice.Get(0)))->GetAddress(0);
  std::cout << "sending packets to address " << addr.GetLocal() << std::endl;
  OnOffHelper onoff ("ns3::UdpSocketFactory",
		     Address (InetSocketAddress (addr.GetLocal(), port))); // TODO Send traffic to AP?
  onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
  onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
  ApplicationContainer apps;

//  onoff.Install(m_clusterNodes.Get(0));

  for(uint32_t i = 0; i< allNodes.GetN(); i++)
    {
//      if(i<2) continue;
      apps.Add(onoff.Install (allNodes.Get (i)));
    }

  apps.Start (Seconds (15));
  apps.Stop (Seconds (100));
}

void Experiment::Initialize ()
{

  /// Initialize Channel and PHY parameters

  SpectrumChannelHelper channelHelper;
  channelHelper.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  channelHelper.AddSpectrumPropagationLoss("ns3::FriisSpectrumPropagationLossModel");
  channelHelper.SetChannel("ns3::MultiModelSpectrumChannel");
  m_channel = channelHelper.Create();


}

NodeContainer Experiment::GetNodes(NodeSpec::NodeType type) const
{
  NodeContainer nodes;
  switch(type)
  {
    case NodeSpec::STA:
      nodes = m_clusterNodes;
      break;
    case NodeSpec::AP:
      nodes = m_apNode;
      break;
    case NodeSpec::RELAY:
      nodes = m_relayNode;
      break;
    default:
      std::cout << "Wrong NodeType provided" << std::endl;
  }
  return nodes;
}

std::map<Ptr<NetDevice>, uint64_t> Experiment::GetPacketsTotal ()
{
  return m_packetsTotal;
}

void Experiment::FixRouting (NodeContainer nodes)
{
  for(uint32_t i = 0; i < nodes.GetN(); i++)
    {
      Ptr<Ipv4> stack = nodes.Get (i)->GetObject<Ipv4> ();
      Ptr<Ipv4RoutingProtocol> rp_Gw = (stack->GetRoutingProtocol ());
      Ptr<Ipv4ListRouting> lrp_Gw = DynamicCast<Ipv4ListRouting> (rp_Gw);
      Ptr<olsr::RoutingProtocol> olsrrp_Gw;

      for (uint32_t i = 0; i < lrp_Gw->GetNRoutingProtocols ();  i++)
        {
          int16_t priority;
          Ptr<Ipv4RoutingProtocol> temp = lrp_Gw->GetRoutingProtocol (i, priority);
          if (DynamicCast<olsr::RoutingProtocol> (temp))
            {
              olsrrp_Gw = DynamicCast<olsr::RoutingProtocol> (temp);
            }
        }

      // Create a special Ipv4StaticRouting instance for RoutingTableAssociation
      // Even the Ipv4StaticRouting instance added to list may be used
      Ptr<Ipv4StaticRouting> hnaEntries = Create<Ipv4StaticRouting> ();

      // Add the required routes into the Ipv4StaticRouting Protocol instance
      // and have the node generate HNA messages for all these routes
      // which are associated with non-OLSR interfaces specified above.
      hnaEntries->AddNetworkRouteTo (Ipv4Address ("10.1.2.0"), Ipv4Mask("255.255.255.0"), uint32_t (2), uint32_t (1));
      olsrrp_Gw->SetRoutingTableAssociation (hnaEntries);
      olsrrp_Gw->AddHostNetworkAssociation(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"));
    }
}

int main (int argc, char *argv[])
{
  CommandLine cmd;

  double psrTh = 0.8;

  cmd.AddValue("psrTh","PSR Threshold that defines the zones", psrTh);

  Experiment experiment;
  experiment.Initialize();


  // Format of input files:
  // n - test cases (e.g., 2)
  // m - nodes in total (e.g., 3)
  // i x y PER upStream - i, seq number; x y coordinates; PER; upStream, node
  // connected


  std::vector<NodeSpec> nodes;
  std::string fileName = "testSpec.txt";
  std::fstream testSpec(fileName, std::ios_base::in);

  int n;
  testSpec >> n;
  std::cout << n << std::endl;



  // NodeSpec ns1;
  // ns1.SetPosition(Vector3D(20,20,0));
  // ns1.SetPsr(1);
  // ns1.SetType(NodeSpec::RELAY);

  // NodeSpec ns2;
  // ns2.SetPosition(Vector3D(40,20,0));
  // ns2.SetPsr(1);
  // ns2.SetType(NodeSpec::STA);

//  NodeSpec ns3;
//  ns3.SetPosition(Vector3D(20,40,0));
//  ns3.SetPsr(1);
//  ns3.SetType(NodeSpec::STA);

  // nodes.push_back(ns1);
  // nodes.push_back(ns2);
//  clusterNodes.push_back(ns3);

  NodeSpec ap;
  ap.SetPosition(Vector3D(0,0,0));
  ap.SetPsr(1); //TODO Should be ignored ?
  ap.SetType(NodeSpec::AP);

  nodes.push_back(ap);

  experiment.CreateNodes(nodes);
  NetDeviceContainer clusterDevices = experiment.CreateCluster();

  //we change channel number here, it has to be this order
  NetDeviceContainer apDevice = experiment.CreateMasterAp();

  experiment.InstallApplications();


//  Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper> (&std::cout);
//  Ipv4RoutingHelper::PrintRoutingTableAt (Seconds(20), clusterDevices.Get(0)->GetNode(), stream, Time::Unit::S);
  Simulator::Stop(Seconds(20));
  Simulator::Run();


  std::map<Ptr<NetDevice>, uint64_t> packetsTotal = experiment.GetPacketsTotal();
  for(std::map<Ptr<NetDevice>, uint64_t>::iterator it = packetsTotal.begin(); it != packetsTotal.end(); it++)
    {
      std::cout <<
	  " at Node " << it->first->GetNode()->GetId() <<
	  " MAC type " << it->first->GetObject<WifiNetDevice>()->GetMac()->GetInstanceTypeId().GetName() <<
	  " Ipv4 Address " << it->first->GetNode()->GetObject<Ipv4L3Protocol>()->
	  GetInterface(it->first->GetNode()->GetObject<Ipv4L3Protocol>()->GetInterfaceForDevice(it->first->GetObject<WifiNetDevice>()))->GetAddress(0).GetLocal () <<
	  " received " << it->second << " packets" << std::endl;
    }



  Simulator::Destroy();

//  std::cout << experiment.GetPacketsTotal().size() << std::endl;

  return 0;
}



