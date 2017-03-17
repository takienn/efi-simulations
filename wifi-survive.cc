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
#include "ns3/netanim-module.h"

#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <ostream>

namespace ns3 {

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
: m_id(0), m_type (STA), m_oPsr (1), m_rPsr(1), m_nPsr(1), m_relayId(0), m_resourceRate (100)
{

}

NodeSpec::NodeSpec (uint32_t id, NodeType type, Vector2D position, uint32_t relayId, double oPsr, double rPsr, double nPsr, double resourceRate)
{
	m_id = id;
	m_type = type;
	m_oPsr = oPsr;
	m_rPsr = rPsr;
	m_nPsr = nPsr;
	m_position = Vector3D(position.x, position.y, 0);
	m_relayId = relayId;
	m_resourceRate = resourceRate;
}

NodeSpec::NodeSpec (uint32_t id, NodeType type, Vector3D position, uint32_t relayId, double oPsr, double rPsr, double nPsr, double resourceRate)
{
	m_id = id;
	m_type = type;
	m_oPsr = oPsr;
	m_rPsr = rPsr;
	m_nPsr = nPsr;
	m_position = position;
	m_relayId = relayId;
	m_resourceRate = resourceRate;
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
NodeSpec::SetPsr (double psr, uint8_t psrType)
{
	switch(psrType)
	{
	case 0:
		m_oPsr = psr;
		break;
	case 1:
		m_rPsr = psr;
		break;
	case 2:
		m_nPsr = psr;
		break;
	default:
		break;
	}
}

double
NodeSpec::GetPsr (uint8_t psrType)
{
	double psr = 0;
	switch(psrType)
	{
	case 0:
		psr = m_oPsr;
		break;
	case 1:
		psr = m_rPsr;
		break;
	case 2:
		psr = m_nPsr;
		break;
	default:
		break;
	}

	return psr;

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

void
NodeSpec::SetRelayId(uint32_t id)
{
	m_relayId = id;
}

uint32_t
NodeSpec::GetRelayId (void)
{
	return m_relayId;
}

void
NodeSpec::SetResourceRate (double rate)
{
	m_resourceRate = rate;
}

double
NodeSpec::GetResourceRate (void)
{
	return m_resourceRate;
}

void
NodeSpec::Print (std::ostream &os) const
{
	NS_LOG_FUNCTION (this << &os);
	os << "id = " << m_id << ", nodeType = " << m_type
			<< ", position = " << m_position << ", OldPsr = " << m_oPsr
			<< ", RelayPsr = " << m_rPsr << ", NewPsr = " << m_nPsr
			<< ", relayId = " << m_relayId;
	if(m_type == 1)
		os << ", resourceRate = " << m_resourceRate;
	os << std::endl;
}

std::ostream& operator<< (std::ostream& os, NodeSpec const& nodeSpec)
{
	nodeSpec.Print (os);
	return os;
}

////////// Implementing EifiTopologyReader /////////////


TypeId
EfiTopologyReader::GetTypeId()
{
	static TypeId tid =
			TypeId ("ns3::EfiTopologyReader")
			.SetGroupName ("TopologyReader")
			.SetParent<TopologyReader> ()
			.AddConstructor<EfiTopologyReader> ();
	return tid;
}
NodeContainer
EfiTopologyReader::Read (void)
{
	return NodeContainer();
}

// NodeID LocationX LocationY PSR RelayID R%
std::vector<std::vector<NodeSpec> >
EfiTopologyReader::ReadNodeSpec (void)
{
	std::ifstream topgen;
	topgen.open (GetFileName ().c_str ());

	std::vector<std::vector<NodeSpec> > nodeSpecsList;

	if (!topgen.is_open ())
	{
		return nodeSpecsList;
	}

	std::istringstream lineBuffer1;
	std::istringstream lineBuffer2;

	std::string line1;
	std::string line2;

	uint32_t id;
	uint32_t type;
	double locX = 0;
	double locY = 0;
	double oPsr = 0; // old Psr
	double rPsr = 0; // relay Psr
	double nPsr = 0; // new Psr
	double resRate = 0;
	uint32_t relayId = 0;


	while (!topgen.eof ())
	{
		line1.clear ();
		line2.clear ();

		lineBuffer1.clear ();
		lineBuffer2.clear ();

		std::vector<NodeSpec> nodeSpecs;

		NodeSpec apNodeSpec(0, NodeSpec::AP, Vector3D(0, 0, 0), relayId, 1, 1, 1, resRate);
		nodeSpecs.push_back(apNodeSpec);
		std::cout << apNodeSpec;

		type = 0;
		id = 0;
		locX = 0;
		locY = 0;
		oPsr = 0;
		rPsr = 0;
		nPsr = 0;
		relayId = 0;
		resRate = 0;

		getline (topgen,line1);
		lineBuffer1.str (line1);
		getline (topgen,line2);
		lineBuffer2.str (line2);

		while(lineBuffer1)
		{
			lineBuffer1 >> id;
			lineBuffer1 >> type;
			if(type == 1)
				lineBuffer2 >> resRate;
			NS_ASSERT(resRate >= 0);
			lineBuffer1 >> locX;
			lineBuffer1 >> locY;
			lineBuffer1 >> oPsr;
			NS_ASSERT(oPsr >= 0);
			lineBuffer1 >> rPsr;
			NS_ASSERT(rPsr >= 0);
			lineBuffer1 >> nPsr;
			NS_ASSERT(nPsr >= 0);
			lineBuffer1 >> relayId; // Put it before the If to make sure lineBuffer1 is advanced
			if(type == 1 || type == 3)
				relayId = 0;

			std::string nodeType;
			if(type == 1)
				nodeType = "RELAY";
			else if(type == 2)
				nodeType = "STA";
			else if(type == 0)
				nodeType = "AP";
			else if(type == 3)
				nodeType = "STA_NORMAL";
			else
			{
				std::cout << "Unrecognizable node type" << std::endl;
				break;
			}

			NodeSpec nodeSpec(id, (NodeSpec::NodeType)type, Vector3D(locX,locY,0), relayId, oPsr*0.01, rPsr*0.01, nPsr*0.01, resRate);
			std::cout << nodeSpec;

			nodeSpecs.push_back(nodeSpec);
		}
		nodeSpecsList.push_back(nodeSpecs);
	}
	return nodeSpecsList;
}

EfiTopologyReader::EfiTopologyReader()
{

}

EfiTopologyReader::~EfiTopologyReader()
{

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
Experiment::SetupNode(Ptr<Node> node, NodeSpec::NodeType type, double psr, uint32_t relayId, bool efiActive)
{
	WifiHelper wifiHelper;
	wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
	//TODO Should the FragmentationThreshold be set here or not down for the client nodes?
	wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager", "FragmentationThreshold", UintegerValue(600)); //TODO The actual value should be confirmed

	SpectrumWifiPhyHelper wifiPhyHelper;
	wifiPhyHelper = SpectrumWifiPhyHelper::Default ();
	//	  wifiPhyHelper.Set ("RxGain", DoubleValue (0.0));
	//	  wifiPhyHelper.Set ("RxNoiseFigure", DoubleValue (0.0));
	//	  wifiPhyHelper.Set ("EnergyDetectionThreshold", DoubleValue (-110.0));
	//	  wifiPhyHelper.Set ("CcaMode1Threshold", DoubleValue (-110.0));
	wifiPhyHelper.SetErrorRateModel("ns3::PsrErrorRateModel", "rate", DoubleValue(psr));
	wifiPhyHelper.SetChannel (m_channel);
	WifiMacHelper macHelper;


	InternetStackHelper internet;
	OlsrHelper olsr;
	Ipv4StaticRoutingHelper staticRouting;
	Ipv4ListRoutingHelper list;
	list.Add (staticRouting, 0);
	list.Add (olsr, 10);
	internet.SetRoutingHelper (list);

	internet.Install(node);

	if(!efiActive)
	{
		wifiPhyHelper.Set ("ChannelNumber", UintegerValue(48));
		std::stringstream ss;
		Ssid ssid;
		ss.str("");
		ss << "MasterAP" << 0;
		ssid = Ssid (ss.str());

		macHelper.SetType ("ns3::StaWifiMac",
				"Ssid", SsidValue (ssid),
				"BE_MaxAmpduSize", UintegerValue(0)); //Disable AMPDU (BE_MaxAmpduSize=0) to make sure Fragmentation Threshold is always used
		NetDeviceContainer device = wifiHelper.Install(wifiPhyHelper, macHelper, node);
		m_clusterDevices[0].Add(device);

		if(m_clusterIpAddress.count(0) == 0)
		{
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address("10.0.0.0"), Ipv4Mask("255.255.255.0"));
			addressHelper.Assign(device);
			m_clusterIpAddress[0] = addressHelper;
		}
		else
		{
			m_relayApIpAddress[0].Assign(device);
		}

		SetupReceivePacket(device.Get(0));
	}
	else switch(type)
	{
	case NodeSpec::STA_NORMAL:
	{
		wifiPhyHelper.Set ("ChannelNumber", UintegerValue(48));
		std::stringstream ss;
		Ssid ssid;
		ss.str("");
		ss << "MasterAP" << relayId;
		ssid = Ssid (ss.str());

		macHelper.SetType ("ns3::StaWifiMac",
				"Ssid", SsidValue (ssid),
				"BE_MaxAmpduSize", UintegerValue(0)); //Disable AMPDU (BE_MaxAmpduSize=0) to make sure Fragmentation Threshold is always used
		NetDeviceContainer device = wifiHelper.Install(wifiPhyHelper, macHelper, node);

		m_relayToApDevice.Add(device);


		if(m_relayApIpAddress.count(relayId) == 0)
		{
			ss.str("");
			ss << "10.0." << relayId << ".0";
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address(ss.str().c_str()), Ipv4Mask("255.255.255.0"));
			addressHelper.Assign(device);
			m_relayApIpAddress[relayId] = addressHelper;
		}
		else
		{
			m_relayApIpAddress[relayId].Assign(device);
		}

		SetupReceivePacket(device.Get(0));
		break;
	}

	case NodeSpec::RELAY:
	{
		wifiPhyHelper.Set ("ChannelNumber", UintegerValue(44));
		// Setting a the Relay as an access point to it's clulser's stations.
		std::stringstream ss;
		Ssid ssid;
		ss << "RelayAP" << node->GetId();
		ssid = Ssid (ss.str());

		macHelper.SetType ("ns3::ApWifiMac",
				"Ssid", SsidValue (ssid));

		NetDeviceContainer device = wifiHelper.Install (wifiPhyHelper, macHelper, node);
		m_relayClusterDevice.Add(device);


		if(m_clusterIpAddress.count(node->GetId()) == 0)
		{
			ss.str("");
			ss << "10.0." << node->GetId() << ".0";
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address(ss.str().c_str()), Ipv4Mask("255.255.255.0"));
			addressHelper.Assign(device);
			m_clusterIpAddress[node->GetId()] = addressHelper;
		}
		else
		{
			m_clusterIpAddress[node->GetId()].Assign(device);
		}

		SetupReceivePacket(device.Get(0));


		wifiPhyHelper.Set ("ChannelNumber", UintegerValue(48));
		ss.str("");
		ss << "MasterAP" << relayId;
		ssid = Ssid (ss.str());

		macHelper.SetType ("ns3::StaWifiMac",
				"Ssid", SsidValue (ssid),
				"BE_MaxAmpduSize", UintegerValue(0)); //Disable AMPDU (BE_MaxAmpduSize=0) to make sure Fragmentation Threshold is always used
		device = wifiHelper.Install(wifiPhyHelper, macHelper, node);

		m_relayToApDevice.Add(device);


		if(m_relayApIpAddress.count(relayId) == 0)
		{
			ss.str("");
			ss << "10.0." << relayId << ".0";
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address(ss.str().c_str()), Ipv4Mask("255.255.255.0"));
			addressHelper.Assign(device);
			m_relayApIpAddress[relayId] = addressHelper;
		}
		else
		{
			m_relayApIpAddress[relayId].Assign(device);
		}

		SetupReceivePacket(device.Get(0));

		break;
	}
	case NodeSpec::AP:
	{
		wifiPhyHelper.Set ("ChannelNumber", UintegerValue(48));
		std::stringstream ss;
		Ssid ssid;
		ss << "MasterAP" << node->GetId();
		ssid = Ssid (ss.str());

		macHelper.SetType ("ns3::ApWifiMac",
				"Ssid", SsidValue (ssid));
		NetDeviceContainer device = wifiHelper.Install(wifiPhyHelper, macHelper, node);
		m_apDevice.Add(device);

		//		  internet.Install(node);

		if(m_relayApIpAddress.count(node->GetId()) == 0)
		{
			ss.str("");
			ss << "10.0." << node->GetId() << ".0";
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address(ss.str().c_str()), Ipv4Mask("255.255.255.0"));
			addressHelper.Assign(device);
			m_relayApIpAddress[node->GetId()] = addressHelper;
		}
		else
		{
			m_relayApIpAddress[node->GetId()].Assign(device);
		}

		SetupReceivePacket(device.Get(0));

		break;
	}
	case NodeSpec::STA:
	{

		// Setting a the Relay as an access point to it's clulser's stations.
		std::stringstream ss;
		Ssid ssid;
		ss << "RelayAP" << relayId;
		ssid = Ssid (ss.str());

		macHelper.SetType ("ns3::StaWifiMac",
				"Ssid", SsidValue (ssid),
				"BE_MaxAmpduSize", UintegerValue(0)); //Disable AMPDU (BE_MaxAmpduSize=0) to make sure Fragmentation Threshold is always used
		NetDeviceContainer device = wifiHelper.Install(wifiPhyHelper, macHelper, node);
		m_clusterDevices[relayId].Add(device);

		//		   internet.Install(node);

		if(m_clusterIpAddress.count(relayId) == 0)
		{
			ss.str("");
			ss << "10.0." << relayId << ".0";
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address(ss.str().c_str()), Ipv4Mask("255.255.255.0"));
			addressHelper.Assign(device);
			m_clusterIpAddress[relayId] = addressHelper;
		}
		else
		{
			m_clusterIpAddress[relayId].Assign(device);
		}

		SetupReceivePacket(device.Get(0));

		break;
	}
	default: break;
	}

}

void
Experiment::CreateNodes (std::vector<NodeSpec> nodeSpecs, bool efiActive)
{
	Initialize();

	for(uint32_t i = 0; i != nodeSpecs.size(); i++)
	{
		Ptr<Node> node = CreateObject<Node> ();
		Ptr<ConstantPositionMobilityModel> mobility = CreateObject<ConstantPositionMobilityModel> ();
		mobility->SetPosition(nodeSpecs[i].GetPosition());
		node->AggregateObject(mobility);
		m_nodePsrValues[node->GetId()] = nodeSpecs[i].GetPsr();

		m_allNodes.Add(node);
		if(nodeSpecs[i].GetType()==NodeSpec::RELAY)
		{
			m_relayNodes[nodeSpecs[i].GetRelayId()].Add(node);
			SetupNode(node, NodeSpec::RELAY, efiActive? nodeSpecs[i].GetPsr(2): nodeSpecs[i].GetPsr(0), nodeSpecs[i].GetRelayId(), efiActive);
		}
		else if(nodeSpecs[i].GetType()==NodeSpec::STA)
		{
			m_clusterNodes[nodeSpecs[i].GetRelayId()].Add(node);
			SetupNode(node, NodeSpec::STA, efiActive? nodeSpecs[i].GetPsr(2): nodeSpecs[i].GetPsr(0), nodeSpecs[i].GetRelayId(), efiActive);

		}
		else if(nodeSpecs[i].GetType()==NodeSpec::AP)
		{
			m_apNodes.Add(node);
			SetupNode(node, NodeSpec::AP, efiActive? nodeSpecs[i].GetPsr(2): nodeSpecs[i].GetPsr(0), nodeSpecs[i].GetRelayId(), efiActive);
		}
		else if(nodeSpecs[i].GetType()==NodeSpec::STA_NORMAL)
		{
			m_relayNodes[nodeSpecs[i].GetRelayId()].Add(node);
			SetupNode(node, NodeSpec::STA_NORMAL, efiActive? nodeSpecs[i].GetPsr(2): nodeSpecs[i].GetPsr(0), nodeSpecs[i].GetRelayId(), efiActive);
		}
	}

	m_clusterIpAddress.clear();
	m_relayApIpAddress.clear();
	m_nodePsrValues.clear();
}

void
Experiment::SetupReceivePacket (Ptr<NetDevice> device)
{
	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
	Ptr<Ipv4L3Protocol> ipv4 = device->GetNode()->GetObject<Ipv4L3Protocol> ();
	Ipv4InterfaceAddress addr = ipv4->GetInterface(ipv4->GetInterfaceForDevice(device))->GetAddress(0);

	InetSocketAddress local = InetSocketAddress (addr.GetLocal(), port);

	Ptr <Socket> sink = Socket::CreateSocket (device->GetNode(), tid);

	sink->Bind (local);
	sink->BindToNetDevice(device);
	sink->SetRecvCallback (MakeCallback ( &Experiment::ReceivePacket, this));

	m_packetsTotal[device] = 0;
}

void LogBusy (Time start, Time duration, WifiPhy::State state)
{
	std::string stateString;
	switch(state)
	{
	case WifiPhy::CCA_BUSY:
		stateString = "CCA_BUSY";
		std::cout << stateString.c_str() << " since " << start.GetSeconds() << " s duration " << duration.GetSeconds() << " s" << std::endl;
		break;
	case WifiPhy::IDLE:
		stateString = "IDLE";
		break;
	case WifiPhy::SLEEP:
		stateString = "SLEEP";
		break;
	case WifiPhy::TX:
		stateString = "TX";
		break;
	case WifiPhy::RX:
		stateString = "RX";
		break;
	case WifiPhy::SWITCHING:
		stateString = "SWITCHING";
		break;
	default:
		NS_FATAL_ERROR("Impossible state");
	}
	//  std::cout << stateString.c_str() << " since " << start.GetSeconds() << " s duration " << duration.GetSeconds() << " s" << std::endl;
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

	while (packet = socket->Recv ())
	{
		m_packetsTotal[device]++;
	}
}

void
Experiment::InstallApplications (NetDeviceContainer src, NetDeviceContainer dst)
{

	NodeContainer allNodes;
	allNodes.Add(m_apNodes);
	//  allNodes.Add(m_clusterNodes);

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	Ptr<Ipv4L3Protocol> ipv4_dst = dst.Get(0)->GetNode()->GetObject<Ipv4L3Protocol> ();
	Ipv4InterfaceAddress addr_dst = ipv4_dst->GetInterface(ipv4_dst->GetInterfaceForDevice(dst.Get(0)))->GetAddress(0);

	Ptr<Ipv4L3Protocol> ipv4_src = src.Get(0)->GetNode()->GetObject<Ipv4L3Protocol> ();
	Ipv4InterfaceAddress addr_src = ipv4_src->GetInterface(ipv4_src->GetInterfaceForDevice(src.Get(0)))->GetAddress(0);

	OnOffHelper onoff ("ns3::UdpSocketFactory",
			Address (InetSocketAddress (addr_dst.GetLocal(), port))); // TODO Send traffic to AP?
	onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
	ApplicationContainer apps;

	//  onoff.Install(m_clusterNodes.Get(0));

	//  apps.Add(onoff.Install (m_apNode));
	//  for(uint32_t i = 0; i< allNodes.GetN(); i++)
	//    {
	////      if(i<2) continue;
	//      apps.Add(onoff.Install (allNodes.Get (i)));
	//    }

	apps.Start (Seconds (15));
	apps.Stop (Seconds (100));

	std::cout << "address " << addr_src.GetLocal() << " sending packets to address " << addr_dst.GetLocal() << std::endl;

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

//TODO fix return variables
NodeContainer Experiment::GetNodes(NodeSpec::NodeType type) const
{
	NodeContainer nodes;
	switch(type)
	{
	case NodeSpec::STA:
		//      nodes = m_clusterNodes;
		break;
	case NodeSpec::AP:
		//      nodes = m_apNodes;
		break;
	case NodeSpec::RELAY:
		//      nodes = m_relayNodes;
		break;
	default:
		std::cout << "Wrong NodeType provided" << std::endl;
	}
	return nodes;
}

NetDeviceContainer Experiment::GetNetDevices(NodeSpec::NodeType type) const
{
	NetDeviceContainer devices;
	switch(type)
	{
	case NodeSpec::STA:
		//      devices = m_clusterDevices;
		break;
	case NodeSpec::AP:
		//      devices = m_apDevice;
		break;
	case NodeSpec::RELAY:
		//      devices = NetDeviceContainer(m_relayClusterDevice, m_relayToApDevice);
		break;
	default:
		std::cout << "Wrong NodeType provided" << std::endl;
	}
	return devices;
}

std::map<Ptr<NetDevice>, uint64_t> Experiment::GetPacketsTotal ()
{
	return m_packetsTotal;
}

void
Experiment::Run(int argc, char *argv[])
{
	double simTime = 20.0;
	std::string animFile = "animation.xml";

	CommandLine cmd;
	cmd.AddValue("simTime","Simulation Time", simTime);
	cmd.AddValue("animFile","Animation xml filename", animFile);


	Simulator::Stop(Seconds(simTime));

	AnimationInterface anim (animFile.c_str());
	anim.EnablePacketMetadata (true);
	anim.SetMobilityPollInterval (Seconds (1));

	for(NodeList::Iterator it = NodeList::Begin(); it != NodeList::End(); it++)
	{
		Ptr<ConstantPositionMobilityModel> mobility = (*it)->GetObject<ConstantPositionMobilityModel>();
		Vector position = mobility->GetPosition();
		anim.SetConstantPosition(*it, position.x, position.y, position.z);
	}
	anim.EnableWifiPhyCounters(Seconds(0), Seconds(simTime), Seconds(1));
	anim.EnableWifiMacCounters(Seconds(0), Seconds(simTime), Seconds(1));
	anim.EnableQueueCounters(Seconds(0), Seconds(simTime), Seconds(1));

	Simulator::Run();


	std::map<Ptr<NetDevice>, uint64_t> packetsTotal = GetPacketsTotal();
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

}

}

using namespace ns3;

int main (int argc, char *argv[])
{

	bool efiActive = true;
	CommandLine cmd;
	cmd.AddValue("efiActive", "normal or Efi mode", efiActive);
	Experiment experiment;
	experiment.Initialize();

	EfiTopologyReader topoReader;
	topoReader.SetFileName("scratch/NS3Input.txt");

	std::vector<std::vector<NodeSpec> > nodesList = topoReader.ReadNodeSpec();

	for(std::vector<std::vector<NodeSpec> >::iterator it = nodesList.begin(); it != nodesList.end(); it++)
	{
		Experiment experiment;
		experiment.CreateNodes(*it, efiActive);
		experiment.Run(argc, argv);
	}

	//	  NetDeviceContainer clusterDevices = experiment.CreateCluster();
	//
	//	  we change channel number here, it has to be this order
	//	  NetDeviceContainer apDevice = experiment.CreateMasterAp();
	//
	//	  experiment.InstallApplications(NetDeviceContainer(experiment.GetNetDevices(NodeSpec::STA).Get(0)),
	//					 NetDeviceContainer(experiment.GetNetDevices(NodeSpec::AP).Get(0)));
	//
	//
	//	  Ptr<OutputStreamWrapper> stream = Create<OutputStreamWrapper> (&std::cout);
	//	  Ipv4RoutingHelper::PrintRoutingTableAt (Seconds(20), clusterDevices.Get(0)->GetNode(), stream, Time::Unit::S);
	//	  GtkConfigStore config;
	//	  config.ConfigureAttributes();


	return 0;
}



