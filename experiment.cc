#include "experiment.h"
#include "ns3/mobility-module.h"
#include "ns3/spectrum-module.h"
#include "ns3/applications-module.h"
#include "ns3/propagation-module.h"
#include "ns3/config-store-module.h"
#include "ns3/olsr-module.h"
#include "ns3/log.h"
#include "ns3/netanim-module.h"
#include "ns3/flow-monitor-module.h"

#include <ctime>

NS_LOG_COMPONENT_DEFINE("EfiExperiment");

using namespace ns3;

uint16_t port = 9;
double g_simTime = 300.0;



Ptr<NetDevice>
GetDeviceOfMac48(Mac48Address addr)
{
	Ptr<NetDevice> device;
	for(NodeList::Iterator it = NodeList::Begin(); it != NodeList::End(); it++)
	{
		Ptr<Node> node = *it;
		for(uint32_t i = 0; i < node->GetNDevices(); i++)
		{
			if(node->GetDevice(i)->GetAddress() == addr)
			{
				device = node->GetDevice(i);
			}
		}
	}
	NS_ASSERT(device);
	return device;
}

Ipv4InterfaceAddress
GetIpv4OfMac48(Mac48Address addr)
{
	Ipv4InterfaceAddress ipv4IfAddress;

	Ptr<NetDevice> device = GetDeviceOfMac48(addr);
	Ptr<Ipv4> ipv4 = device->GetNode()->GetObject<Ipv4> ();
	ipv4IfAddress = ipv4->GetAddress(ipv4->GetInterfaceForDevice(device), 0);

	return ipv4IfAddress;
}


void
Experiment::EnableActiveProbing(Ptr<StaWifiMac> staMac)
{
	Simulator::Schedule(Seconds(m_rand->GetValue(1,10)), &ObjectBase::SetAttribute, staMac, "ActiveProbing", BooleanValue(true));
}

void
DisableActiveProbing(Ptr<StaWifiMac> staMac)
{
	staMac->SetAttribute("ActiveProbing", BooleanValue(false));
}

static void
LogEnqueueAction (Experiment* experiment, Ptr<NetDevice> device, Ptr<WifiMacQueueItem const> item)
{
  experiment->UpdateQueueEnqueue(device);
}

static void
LogDequeueAction (Experiment* experiment, Ptr<NetDevice> device, Ptr<WifiMacQueueItem const> item)
{
  Time waited = Now() - item->GetTimeStamp();
  experiment->UpdateQueueWait(device, waited);

}

static void
LogQueueDropAction (Experiment* experiment, Ptr<NetDevice> device, Ptr<WifiMacQueueItem const> item)
{
  Time waited = Now() - item->GetTimeStamp();
  experiment->UpdateQueueDrop(device, waited);
}

void
Experiment::SetupQueueMonitoring(Ptr<NetDevice> device)
{
  Ptr<RegularWifiMac> mac = device->GetObject<WifiNetDevice>()->GetMac()->GetObject<RegularWifiMac>();
	PointerValue ptr;
	Ptr<WifiMacQueue> wifiMacQueue;

	mac->GetAttribute("DcaTxop", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<DcaTxop>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeBoundCallback(&LogEnqueueAction, this, device));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeBoundCallback(&LogDequeueAction, this, device));
	wifiMacQueue->TraceConnectWithoutContext("Drop", MakeBoundCallback(&LogQueueDropAction, this, device));

//	m_queueWaitRecord["DcaTxop"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[device] = std::pair<Time, uint64_t>(Seconds(0),0);


	mac->GetAttribute("BK_EdcaTxopN", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<EdcaTxopN>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeBoundCallback(&LogEnqueueAction, this, device));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeBoundCallback(&LogDequeueAction, this, device));
//	m_queueWaitRecord["BK_EdcaTxopN"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[device] = std::pair<Time, uint64_t>(Seconds(0),0);


	mac->GetAttribute("VI_EdcaTxopN", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<EdcaTxopN>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeBoundCallback(&LogEnqueueAction, this, device));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeBoundCallback(&LogDequeueAction, this, device));
//	m_queueWaitRecord["VI_EdcaTxopN"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[device] = std::pair<Time, uint64_t>(Seconds(0),0);


	mac->GetAttribute("VO_EdcaTxopN", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<EdcaTxopN>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeBoundCallback(&LogEnqueueAction, this, device));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeBoundCallback(&LogDequeueAction, this, device));
//	m_queueWaitRecord["VO_EdcaTxopN"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[device] = std::pair<Time, uint64_t>(Seconds(0),0);


	mac->GetAttribute("BE_EdcaTxopN", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<EdcaTxopN>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeBoundCallback(&LogEnqueueAction, this, device));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeBoundCallback(&LogDequeueAction, this, device));
//	m_queueWaitRecord["BE_EdcaTxopN"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[device] = std::pair<Time, uint64_t>(Seconds(0),0);

}

void
SimulationProgress()
{
	NS_LOG_UNCOND("Simulation time: " << Now().GetSeconds() << std::endl);
	Simulator::Schedule(Seconds(5.0), &SimulationProgress);
}

Experiment::Experiment ()
{
	Initialize();
}

Experiment::~Experiment()
{
}

void
Experiment::LogAssoc (Mac48Address sta, Mac48Address addr)
{
	NS_LOG_INFO(sta << "=" << GetIpv4OfMac48(sta).GetLocal()
			<< " Associated with " << addr << "=" << GetIpv4OfMac48(addr).GetLocal()
			<< " at time " << Now().GetSeconds()
			<< std::endl);

	m_relayAssocTable[GetDeviceOfMac48(addr)->GetNode()->GetId()]++;
}

void
Experiment::LogDeAssoc (Mac48Address sta, Mac48Address addr)
{
	NS_LOG_INFO(sta << "=" << GetIpv4OfMac48(sta).GetLocal()
			<< " DeAssociated with " << addr << "=" << GetIpv4OfMac48(addr).GetLocal()
			<< " at time " << Now().GetSeconds()
			<< std::endl);
	m_relayAssocTable[GetDeviceOfMac48(addr)->GetNode()->GetId()]++;
}

void
Experiment::ClusterWakeup(uint32_t id, Time time)
{
	Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();

	for(uint32_t i = 0; i < m_relayClusterDevice.GetN(); i++)
	{
		if (id == m_relayClusterDevice.Get(i)->GetNode()->GetId())
		{
			Ptr<WifiNetDevice> device = m_relayClusterDevice.Get(i)->GetObject<WifiNetDevice>();
			Ptr<WifiPhy> staPhy = device->GetPhy ();
			Simulator::Schedule (time, &WifiPhy::ResumeFromSleep, staPhy);
			NS_LOG_INFO("waiking up " << GetIpv4OfMac48(device->GetMac()->GetAddress()).GetLocal()
					<< " at " << time.GetSeconds());
			break;
		}
	}


	NetDeviceContainer devices = m_clusterDevices[id];
	for(NetDeviceContainer::Iterator it = devices.Begin(); it != devices.End(); it++)
	{

			Ptr<WifiNetDevice> device = (*it)->GetObject<WifiNetDevice>();
			Ptr<WifiPhy> staPhy = device->GetPhy ();
			Simulator::Schedule (time + MilliSeconds(rand->GetValue(0, 1000)), &WifiPhy::ResumeFromSleep, staPhy);
			NS_LOG_INFO("waiking up " << GetIpv4OfMac48(device->GetMac()->GetAddress()).GetLocal()
					<< " at " << time.GetSeconds());
	}
}

void
Experiment::ClusterSleep(uint32_t id, Time time)
{
	for(uint32_t i = 0; i < m_relayClusterDevice.GetN(); i++)
	{
		if (id == m_relayClusterDevice.Get(i)->GetNode()->GetId())
		{
			Ptr<WifiNetDevice> device = m_relayClusterDevice.Get(i)->GetObject<WifiNetDevice>();
			Ptr<WifiPhy> staPhy = device->GetPhy ();
			Simulator::Schedule (time, &WifiPhy::SetSleepMode, staPhy);

			NS_LOG_INFO("putting to sleep " << GetIpv4OfMac48(device->GetMac()->GetAddress()).GetLocal() << " at " << time.GetSeconds());
			break;
		}
	}

	NetDeviceContainer devices = m_clusterDevices[id];
	for(NetDeviceContainer::Iterator it = devices.Begin(); it != devices.End(); it++)
	{

			Ptr<WifiNetDevice> device = (*it)->GetObject<WifiNetDevice>();
			Ptr<WifiPhy> staPhy = device->GetPhy ();
			Simulator::Schedule (time, &WifiPhy::SetSleepMode, staPhy);
			NS_LOG_INFO("putting to sleep " << GetIpv4OfMac48(device->GetMac()->GetAddress()).GetLocal() << " at " << time.GetSeconds());
	}
}
void
Experiment::SetupNode(Ptr<Node> node, NodeSpec::NodeType type, double psr, uint32_t relayId, double resRate, bool efiActive)
{
	WifiHelper wifiHelper;
	wifiHelper.SetStandard(WIFI_PHY_STANDARD_80211n_5GHZ);
	//TODO Should the FragmentationThreshold be set here or not down for the client nodes?
	wifiHelper.SetRemoteStationManager("ns3::ConstantRateWifiManager",
			"DataMode", StringValue("HtMcs15"), "ControlMode", StringValue("HtMcs15"));

//			"FragmentationThreshold", UintegerValue(600)); //TODO The actual value should be confirmed

	SpectrumWifiPhyHelper wifiPhyHelper;
	wifiPhyHelper = SpectrumWifiPhyHelper::Default ();
	wifiPhyHelper.Set ("ShortGuardEnabled", BooleanValue (true));
	wifiPhyHelper.Set ("Antennas", UintegerValue (2));
	wifiPhyHelper.Set ("MaxSupportedTxSpatialStreams", UintegerValue (2));
	wifiPhyHelper.Set ("MaxSupportedRxSpatialStreams", UintegerValue (2));
	//	  wifiPhyHelper.Set ("RxGain", DoubleValue (0.0));
	//	  wifiPhyHelper.Set ("RxNoiseFigure", DoubleValue (0.0));
	//	  wifiPhyHelper.Set ("EnergyDetectionThreshold", DoubleValue (-110.0));
	//	  wifiPhyHelper.Set ("CcaMode1Threshold", DoubleValue (-110.0));
//	wifiPhyHelper.SetErrorRateModel("ns3::PsrErrorRateModel", "rate", DoubleValue(psr));
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

		Ipv4InterfaceContainer ipv4IfContainer;

		if(m_relayApIpAddress.count(relayId) == 0)
		{
			ss.str("");
			ss << "10.0." << relayId << ".0";
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address(ss.str().c_str()), Ipv4Mask("255.255.255.0"));
			ipv4IfContainer = addressHelper.Assign(device);
			m_relayApIpAddress[relayId] = addressHelper;
		}
		else
		{
			ipv4IfContainer = m_relayApIpAddress[relayId].Assign(device);
		}

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
				"Ssid", SsidValue (ssid),
				"EnableBeaconJitter", BooleanValue(true),
				"BeaconGeneration", BooleanValue(true));

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
//
		wifiPhyHelper.Set ("ChannelNumber", UintegerValue(48));
		ss.str("");
		ss << "MasterAP" << relayId;
		ssid = Ssid (ss.str());

		macHelper.SetType ("ns3::StaWifiMac",
				"Ssid", SsidValue (ssid),
				"BE_MaxAmpduSize", UintegerValue(0)); //Disable AMPDU (BE_MaxAmpduSize=0) to make sure Fragmentation Threshold is always used
		device = wifiHelper.Install(wifiPhyHelper, macHelper, node);

		m_relayToApDevice.Add(device);

		Ipv4InterfaceContainer ipv4IfContainer;

		if(m_relayApIpAddress.count(relayId) == 0)
		{
			ss.str("");
			ss << "10.0." << relayId << ".0";
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address(ss.str().c_str()), Ipv4Mask("255.255.255.0"));
			ipv4IfContainer = addressHelper.Assign(device);
			m_relayApIpAddress[relayId] = addressHelper;
		}
		else
		{
			ipv4IfContainer = m_relayApIpAddress[relayId].Assign(device);
		}

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


		break;
	}
	case NodeSpec::STA:
	{
		// Setting a the Relay as an access point to it's clulser's stations.
		std::stringstream ss;
		Ssid ssid;
		ss << "RelayAP" << relayId;
		ssid = Ssid (ss.str());

		wifiPhyHelper.Set ("ChannelNumber", UintegerValue(44));

		macHelper.SetType ("ns3::StaWifiMac",
				"Ssid", SsidValue (ssid));//				"BE_MaxAmpduSize", UintegerValue(0)); // Disable AMPDU (BE_MaxAmpduSize=0) to make sure Fragmentation Threshold is always used
		NetDeviceContainer device = wifiHelper.Install(wifiPhyHelper, macHelper, node);

		m_clusterDevices[relayId].Add(device);

		Ipv4InterfaceContainer ipv4IfContainer;

		if(m_clusterIpAddress.count(relayId) == 0)
		{
			ss.str("");
			ss << "10.0." << relayId << ".0";
			Ipv4AddressHelper addressHelper;

			addressHelper.SetBase(Ipv4Address(ss.str().c_str()), Ipv4Mask("255.255.255.0"));
			ipv4IfContainer = addressHelper.Assign(device);
			m_clusterIpAddress[relayId] = addressHelper;
		}
		else
		{
			ipv4IfContainer = m_clusterIpAddress[relayId].Assign(device);
		}
		break;
	}
	default: break;
	}

}


void
Experiment::CreateNodes (std::vector<NodeSpec> nodeSpecs, bool efiActive)
{

	for(uint32_t i = 0; i != nodeSpecs.size(); i++)
	{
		Ptr<Node> node = CreateObject<Node> ();
		NS_ASSERT(node->GetId() == nodeSpecs[i].GetId());
		m_nodePsrValues[node->GetId()] = nodeSpecs[i].GetPsr(2);

		Ptr<ConstantPositionMobilityModel> mobility = CreateObject<ConstantPositionMobilityModel> ();
		mobility->SetPosition(nodeSpecs[i].GetPosition());
		node->AggregateObject(mobility);

		if(nodeSpecs[i].GetResourceRate()!=0)
			m_relayResourceMap[nodeSpecs[i].GetId()] = nodeSpecs[i].GetResourceRate();

		m_allNodes.Add(node);
		if(nodeSpecs[i].GetType()==NodeSpec::RELAY)
		{
			m_relayNodes[nodeSpecs[i].GetRelayId()].Add(node);
			SetupNode(node, NodeSpec::RELAY, efiActive? nodeSpecs[i].GetPsr(2): nodeSpecs[i].GetPsr(0), nodeSpecs[i].GetRelayId(), nodeSpecs[i].GetResourceRate(), efiActive);
		}
		else if(nodeSpecs[i].GetType()==NodeSpec::STA)
		{
			m_clusterNodes[nodeSpecs[i].GetRelayId()].Add(node);
			SetupNode(node, NodeSpec::STA, efiActive? nodeSpecs[i].GetPsr(2): nodeSpecs[i].GetPsr(0), nodeSpecs[i].GetRelayId(), nodeSpecs[i].GetResourceRate(), efiActive);

		}
		else if(nodeSpecs[i].GetType()==NodeSpec::AP)
		{
			m_apNodes.Add(node);
			SetupNode(node, NodeSpec::AP, efiActive? nodeSpecs[i].GetPsr(2): nodeSpecs[i].GetPsr(0), nodeSpecs[i].GetRelayId(), nodeSpecs[i].GetResourceRate(),  efiActive);
		}
		else if(nodeSpecs[i].GetType()==NodeSpec::STA_NORMAL)
		{
			m_relayNodes[nodeSpecs[i].GetRelayId()].Add(node);
			SetupNode(node, NodeSpec::STA_NORMAL, efiActive? nodeSpecs[i].GetPsr(2): nodeSpecs[i].GetPsr(0), nodeSpecs[i].GetRelayId(), nodeSpecs[i].GetResourceRate(),  efiActive);
		}
	}

	for(std::map<uint32_t, NodeContainer>::iterator it = m_relayNodes.begin(); it!=m_relayNodes.end(); it++)
	{
		for(NodeContainer::Iterator nIt = it->second.Begin(); nIt != it->second.End(); nIt++)
		{
			uint32_t infN = (*nIt)->GetObject<Ipv4>()->GetNInterfaces();
			NS_LOG_INFO("RELAY " << "address " << (*nIt)->GetObject<Ipv4>()->GetAddress(infN - 1,0).GetLocal() // TODO Because we always create the AP interface before the STA interface
							<< " connected to AP address " << m_apNodes.Get(0)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal()
							<< std::endl);
		}

	}
	for(std::map<uint32_t, NodeContainer>::iterator it = m_clusterNodes.begin(); it!= m_clusterNodes.end(); it++)
	{
		for(NodeContainer::Iterator nIt = it->second.Begin(); nIt != it->second.End(); nIt++)
		{
			NS_LOG_INFO("STA " << "address " << (*nIt)->GetObject<Ipv4>()->GetAddress(1,0).GetLocal()
						<< " connected to RELAY address " << NodeList::GetNode(it->first)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal()
						<< std::endl);
		}
	}

	for(NodeContainer::Iterator it = m_relayNodes[0].Begin(); it!= m_relayNodes[0].End(); it++)
	{
		uint32_t id = (*it)->GetId();

		ClusterSleep(id, Now());
	}
	Config::Set ("/NodeList/*/DeviceList/*/$ns3::WifiNetDevice/Phy/ChannelWidth", UintegerValue (20));
	m_clusterIpAddress.clear();
	m_relayApIpAddress.clear();
}


void
Experiment::SetupReceivePacket (Ptr<NetDevice> device)
{
	TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
	Ptr<Ipv4L3Protocol> ipv4 = device->GetNode()->GetObject<Ipv4L3Protocol> ();
	Ipv4InterfaceAddress addr = ipv4->GetInterface(ipv4->GetInterfaceForDevice(device))->GetAddress(0);

	InetSocketAddress local = InetSocketAddress (addr.GetLocal(), port);

	Ptr <Socket> sink = Socket::CreateSocket (device->GetNode(), tid);
	sink->SetAllowBroadcast(true);

	sink->Bind (local);
	sink->BindToNetDevice(device);
	sink->SetRecvCallback (MakeCallback ( &Experiment::ReceivePacket, this));
}

void Experiment::LogBusy (Time start, Time duration, WifiPhy::State state)
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
//		Ipv4Address ipaddr =  GetIpv4OfMac48(device->GetObject<WifiNetDevice>()->GetMac()->GetObject<StaWifiMac>()->GetAddress()).GetLocal();
	    if(m_rand->GetValue() > 1 - m_nodePsrValues[device->GetNode()->GetId()])//TODO
	      m_packetsTotal[device] += packet->GetSize();
	}
}

//TODO This install an app sending traffic from every netdevice in src to every netdevice in dst
ApplicationContainer
Experiment::InstallApplications (NetDeviceContainer src, NetDeviceContainer dst)
{

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	ApplicationContainer apps;

	for(NetDeviceContainer::Iterator it1 = src.Begin(); it1 != src.End(); it1++)
	{
	    for(NetDeviceContainer::Iterator it2 = dst.Begin(); it2 != dst.End(); it2++)
	      {
		Ptr<Ipv4L3Protocol> ipv4_dst = (*it2)->GetNode()->GetObject<Ipv4L3Protocol> ();
		Ipv4InterfaceAddress addr_dst = ipv4_dst->GetInterface(ipv4_dst->GetInterfaceForDevice(*it2))->GetAddress(0);

		Ptr<Ipv4L3Protocol> ipv4_src = (*it1)->GetNode()->GetObject<Ipv4L3Protocol> ();
//		Ipv4InterfaceAddress addr_src = ipv4_src->GetInterface(ipv4_src->GetInterfaceForDevice(*it1))->GetAddress(0);

		OnOffHelper onoff ("ns3::UdpSocketFactory",
				Address (InetSocketAddress (addr_dst.GetLocal(), port))); // TODO Send traffic to AP?
		onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
		onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
		onoff.SetAttribute ("PacketSize", UintegerValue(1472)); // Total IP packet size of 1500
		onoff.SetConstantRate(DataRate("150Mbps"));
		apps.Add(onoff.Install((*it1)->GetNode()));
	      }
	}

	return apps;
}

ApplicationContainer
Experiment::InstallApplications(uint32_t relayId)
{
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	ApplicationContainer apps;


	Ptr<Ipv4L3Protocol> ipv4_src = NodeList::GetNode(relayId)->GetObject<Ipv4L3Protocol> ();
	Ptr<NetDevice> relayDevice;
	for(uint32_t i = 0; i < m_relayClusterDevice.GetN(); i++)
	{
		if(m_relayClusterDevice.Get(i)->GetNode()->GetId() == relayId)
			{
				relayDevice = m_relayClusterDevice.Get(i);
				break;
			}
	}
	NS_ASSERT(relayDevice);

//	Ipv4InterfaceAddress addr_src = ipv4_src->GetInterface(ipv4_src->GetInterfaceForDevice(relayDevice))->GetAddress(0);

	NetDeviceContainer clusterDevices = m_clusterDevices[relayId];
	NS_ASSERT(clusterDevices.GetN());

	for(NetDeviceContainer::Iterator it = clusterDevices.Begin(); it != clusterDevices.End(); it++)
	{
		Ptr<Ipv4L3Protocol> ipv4_dst = (*it)->GetNode()->GetObject<Ipv4L3Protocol> ();
		Ipv4InterfaceAddress addr_dst = ipv4_dst->GetInterface(ipv4_dst->GetInterfaceForDevice(*it))->GetAddress(0);

		OnOffHelper onoff ("ns3::UdpSocketFactory",
				Address (InetSocketAddress (addr_dst.GetLocal(), port))); // TODO Send traffic to AP?
		onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
		onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
		onoff.SetAttribute ("PacketSize", UintegerValue(1472)); // Total IP packet size of 1500
		onoff.SetConstantRate(DataRate("150Mbps"));
		apps.Add(onoff.Install(relayDevice->GetNode()));
	}

	return apps;
}

static void
MonitorPhyTx (Experiment* experiment, Ptr<NetDevice> device, Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu)
{
  experiment->UpdatePhyTxBytes(device, packet->GetSize());
}

void
EnableBroadcast(Ptr<Application> app)
{
  Ptr<OnOffApplication> onoffApp = DynamicCast<OnOffApplication>(app);
    Ptr<Socket> socket = onoffApp->GetSocket();
    socket->SetAllowBroadcast(true);
}

void
Experiment::UpdatePhyTxBytes(Ptr<NetDevice> device, double value)
{
  m_totalPhyTxBytes[device] += value;
}

void
Experiment::UpdateQueueWait(Ptr<NetDevice> device, Time time)
{
  m_queueWaitRecord[device].first += time;
  m_queueWaitRecord[device].second++;
}

void
Experiment::UpdateQueueDrop(Ptr<NetDevice> device, Time time)
{
  m_queueDropRecord[device].first += time;
  m_queueDropRecord[device].second++;
}

void
Experiment::UpdateQueueEnqueue(Ptr<NetDevice> device)
{
  m_queueEnqueueRecord[device]++;
}

ApplicationContainer
Experiment::InstallApplications (NetDeviceContainer src, Ipv4Address address)
{

	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	ApplicationContainer apps;

	for(NetDeviceContainer::Iterator it1 = src.Begin(); it1 != src.End(); it1++)
	  {

	    Ptr<Ipv4L3Protocol> ipv4_src = (*it1)->GetNode()->GetObject<Ipv4L3Protocol> ();
	    Ipv4InterfaceAddress addr_src = ipv4_src->GetInterface(ipv4_src->GetInterfaceForDevice(*it1))->GetAddress(0);

	    OnOffHelper onoff ("ns3::UdpSocketFactory",
			       Address (InetSocketAddress (address, port))); // TODO Send traffic to AP?
	    onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
	    onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));

	    ApplicationContainer appContainer = onoff.Install((*it1)->GetNode());
	    Ptr<Application> onoffApp = appContainer.Get(0);
	    Simulator::Schedule(Seconds(15)+NanoSeconds(1), EnableBroadcast, onoffApp);


	    apps.Add(appContainer);

	    std::cout << "address " << addr_src.GetLocal() << "("
	    		<< (src.Get(0)->GetObject<WifiNetDevice>()->GetMac()->GetObject<ApWifiMac>() == 0? "STA":"Relay")
				<< ") sending packets to cluster node with address " << address << std::endl;

	  }

	return apps;
}


void Experiment::Initialize ()
{

	/// Initialize Channel and PHY parameters

	SpectrumChannelHelper channelHelper;
	channelHelper.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
	channelHelper.AddSpectrumPropagationLoss("ns3::ConstantSpectrumPropagationLossModel");
	channelHelper.SetChannel("ns3::MultiModelSpectrumChannel");
	m_channel = channelHelper.Create();

	m_rand = CreateObject<UniformRandomVariable>();
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
    case NodeSpec::STA_NORMAL:
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
    case NodeSpec::STA_NORMAL:
      break;
    case NodeSpec::AP:
      //      devices = m_apDevice;
      break;
    case NodeSpec::RELAY:
      //      devices = NetDeviceContainer(m_relayClusterDevice, m_relayToApDevice);
      break;
    default:
      NS_FATAL_ERROR("Wrong NodeType provided" << std::endl);
  }
  return devices;
}

void
Experiment::ResetStats()
{
	m_totalPhyTxBytes.clear();
	m_packetsTotal.clear();
	m_queueWaitRecord.clear();
	m_queueEnqueueRecord.clear();
	m_queueDropRecord.clear();
}


bool
Experiment::ClientsAssociated (uint32_t id)
{
	if(m_clusterNodes[id].GetN() > m_relayAssocTable[id])
		return false;

	NS_LOG_INFO(m_relayAssocTable[id] << " Clients connected to relay " << id << std::endl);
	return true;
}

void
Experiment::SetupHooks (NetDeviceContainer devices)
{
	for(uint32_t i = 0; i < devices.GetN(); i++)
	{
		Ptr<RegularWifiMac> mac = devices.Get(i)->GetObject<WifiNetDevice> ()->GetMac()->GetObject<RegularWifiMac> ();
		if(mac->GetObject<StaWifiMac>())
		  {
		    mac->SetAttribute("MaxMissedBeacons", UintegerValue(100000));
		    mac->TraceConnectWithoutContext("DeAssoc", MakeCallback(&Experiment::LogDeAssoc, this));
		    mac->TraceConnectWithoutContext("Assoc", MakeCallback(&Experiment::LogAssoc, this));

		  }

		SetupQueueMonitoring(devices.Get(i));

		Ptr<WifiPhy> phy = devices.Get(i)->GetObject<WifiNetDevice>()->GetPhy();
		phy->TraceConnectWithoutContext("MonitorSnifferTx", MakeBoundCallback(&MonitorPhyTx, this, devices.Get(i)));

		SetupReceivePacket(devices.Get(i));
	}

}

void
Experiment::SetupPsr(NetDeviceContainer devices, double val = 2)
{
  for(uint32_t i = 0; i < devices.GetN(); i++)
    {
      Ptr<WifiPhy> phy = devices.Get(i)->GetObject<WifiNetDevice>()->GetPhy();
      Ptr<PsrErrorRateModel> err = CreateObject<PsrErrorRateModel> ();
      double psr = m_nodePsrValues[devices.Get(i)->GetNode()->GetId()];
      if(val>1)
	err->SetRate(psr);
      else
	err->SetRate(val);
      phy->SetErrorRateModel(err);
    }
}
void
Experiment::Run(bool downlink)
{
  std::cout.flush();

  Simulator::Schedule(Now(), &SimulationProgress);
  for(NetDeviceContainer::Iterator rit = m_relayClusterDevice.Begin(); rit!= m_relayClusterDevice.End(); rit++)
    {
      uint32_t id = (*rit)->GetNode()->GetId();
      double resRate = m_relayResourceMap[id]; // Apparently total 100 seconds is not enough

      std::cout << "\n--- Running Downlink at Cluster " << id << " Resources " << resRate << "s ---\n";
      std::cout.flush();
      std::cout << "Clients in this cluster\n";
      std::cout.flush();
      NodeContainer nodes = m_clusterNodes[id];
      for(uint32_t i = 0; i < nodes.GetN(); i++)
	{
	  Ptr<Ipv4> ipv4 = nodes.Get(i)->GetObject<Ipv4>();
	  std::cout << ipv4->GetAddress(1,0).GetLocal() << " PSR = " << m_nodePsrValues[nodes.Get(i)->GetId()] << "\n";
	  std::cout.flush();
	}

      SetupHooks (*rit);
      SetupHooks (m_clusterDevices[id]);
//
//      SetupPsr(m_clusterDevices[id], 1);
//      SetupPsr(NetDeviceContainer(*rit), 1);

      ClusterWakeup(id, Now());
      while(!ClientsAssociated(id))
	{
	  Simulator::Stop(Seconds(10));
	  Simulator::Run();
	}
      ResetStats();
//
//      SetupPsr(m_clusterDevices[id]);
//      SetupPsr(NetDeviceContainer(*rit));
      if(downlink)
	NS_LOG_UNCOND("Downlink traffic started");
      else
	NS_LOG_UNCOND("Uplink traffic started");

      Simulator::Stop(Seconds(resRate));

      Ptr<NetDevice> relayDevice = NodeList::GetNode(id)->GetDevice(1);
      ApplicationContainer apps;
      if(downlink)
	{
	  apps = InstallApplications(NetDeviceContainer(relayDevice), m_clusterDevices[id]);
	}
      else
	{
	  apps = InstallApplications(m_clusterDevices[id], NetDeviceContainer(relayDevice));
	}
      //		ApplicationContainer apps = InstallApplications(id);
      apps.Start(Seconds(0));
      apps.Stop(Seconds(resRate));


      Simulator::Run();
      ClusterSleep(id, Now());

      NetDeviceContainer devices = m_clusterDevices[id];
      if(downlink)
	{
	  Ipv4InterfaceAddress address;

	  Ptr<NetDevice> device = *rit;
	  Ptr<Ipv4> ipv4 = device->GetNode()->GetObject<Ipv4>();
	  address = ipv4->GetAddress(ipv4->GetInterfaceForDevice(device), 0);

	  std::pair<Time, uint64_t> pair = m_queueWaitRecord[device];
	  double time = pair.first.GetSeconds();
	  double items = pair.second;
	  double queueAvgWait = (items == 0 ? 0:time/items);

	  pair = m_queueDropRecord[device];
	  time = pair.first.GetSeconds();
	  items = pair.second;
	  double queueAvgDropTime = (items == 0 ? 0:time/items);

	  uint64_t droped = m_queueEnqueueRecord[device];
	  double dropRate = items/droped;
	  std::cout << "RelayID " << id << "\n"
	      << "RelayIP " << address.GetLocal() << "\n"
	      << "AvgQueueWait " << queueAvgWait << "s\n"
	      << "AvgQueueDropTime " << queueAvgDropTime << "s\n"
	      << "QueueDropRate " << 100*dropRate << "%\n"
	      << "ClusterDownlinkThroughput " << m_totalPhyTxBytes[*rit]/resRate/1.0e6 << "MB/s\n";
	  std::cout.flush();

	  for(uint32_t i = 0; i < devices.GetN(); i++)
	    {
	      std::cout << "ThroughputClient " << devices.Get(i)->GetNode()->GetObject<Ipv4>()->GetAddress(1,0).GetLocal();
	      double throughput = m_packetsTotal[devices.Get(i)]/resRate/1.0e6;
	      std::cout << " " << throughput << "MB/s\n";
	      std::cout.flush();
	    }
	  std::cout << std::endl;
	  NS_LOG_UNCOND("--- Finished Running Downlink at Cluster " << id << " ---");
	}
      else
	{
	  for(uint32_t i = 0; i < devices.GetN(); i++)
	    {
	      std::cout << "Client " << devices.Get(i)->GetNode()->GetObject<Ipv4>()->GetAddress(1,0).GetLocal()
			<< std::endl;
	      std::pair<Time, uint64_t> pair = m_queueWaitRecord[devices.Get(i)];
	      double time = pair.first.GetSeconds();
	      double items = pair.second;
	      double queueAvgWait = (items == 0 ? 0:time/items);

	      pair = m_queueDropRecord[devices.Get(i)];
	      time = pair.first.GetSeconds();
	      items = pair.second;
	      double queueAvgDropTime = (items == 0 ? 0:time/items);

	      uint64_t droped = m_queueEnqueueRecord[devices.Get(i)];
	      double dropRate = items/droped;
	      std::cout << "AvgQueueWait " << queueAvgWait << "s\n";
	      std::cout << "AvgQueueDropTime " << queueAvgDropTime << "s\n";
	      std::cout << "QueueDropRate " << 100*dropRate << "%\n";
	      double throughput = m_totalPhyTxBytes[devices.Get(i)]/resRate/1.0e6;
	      std::cout << "Throughput " << throughput << "MB/s\n";
	      Ptr<NetDevice> device = devices.Get(i);
	      std::cout << " " << throughput << "MB/s\n";
	      std::cout.flush();
	    }
	  std::cout << "RelayRxThroughput " << m_packetsTotal[*rit]/resRate/1.0e6 << "MB/s"
		    << std::endl;

	  NS_LOG_UNCOND("--- Finished Running Uplink at Cluster " << id << " ---");
	}
      ResetStats();


      std::cout.flush();

    }


  Simulator::Destroy();
}



