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

void
Experiment::LogDcaTxopEnqueueAction (Ptr<WifiMacQueueItem const> item)
{
//	NS_LOG_INFO("DcaTxop Queue Enqueue action Item "<< item->GetPacket()->GetUid()
//			<< " at " << Now().GetSeconds()
//			<< std::endl);
}

void
Experiment::LogDcaTxopDequeueAction (Ptr<WifiMacQueueItem const> item)
{
	Time waited = Now() - item->GetTimeStamp();
//	NS_LOG_INFO("DcaTxop Queue Dequeue action at " << Now().GetSeconds()
//			<< " Item " << item->GetPacket()->GetUid()
//			<< " waited for " << waited.GetMicroSeconds() << "us"
//			<< std::endl);

//	m_queueWaitRecord["DcaTxop"] = std::pair<Time, uint64_t>(m_queueWaitRecord["DcaTxop"].first+waited, m_queueWaitRecord["DcaTxop"].second+1);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(m_queueWaitRecord[0].first+waited, m_queueWaitRecord[0].second+1);

}

void
Experiment::LogVO_EdcaTxopNEnqueueAction (Ptr<WifiMacQueueItem const> item)
{
//	NS_LOG_INFO("VO_EdcaTxopN Queue Enqueue action Item "<< item->GetPacket()->GetUid()
//			<< " at " << Now().GetSeconds()
//			<< std::endl);
}

void
Experiment::LogVO_EdcaTxopNDequeueAction (Ptr<WifiMacQueueItem const> item)
{
	Time waited = Now() - item->GetTimeStamp();

//	NS_LOG_INFO("VO_EdcaTxopN Queue Dequeue action at " << Now().GetSeconds()
//			<< " Item " << item->GetPacket()->GetUid()
//			<< " waited for " << (Now() - item->GetTimeStamp()).GetMicroSeconds() << "us"
//			<< std::endl);

//	m_queueWaitRecord["VO_EdcaTxopN"] = std::pair<Time, uint64_t>(m_queueWaitRecord["VO_EdcaTxopN"].first+waited, m_queueWaitRecord["VO_EdcaTxopN"].second+1);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(m_queueWaitRecord[0].first+waited, m_queueWaitRecord[0].second+1);

}

void
Experiment::LogVI_EdcaTxopNEnqueueAction (Ptr<WifiMacQueueItem const> item)
{
	NS_LOG_INFO("VI_EdcaTxopN Queue Enqueue action Item "<< item->GetPacket()->GetUid()
			<< " at " << Now().GetSeconds()
			<< std::endl);

}

void
Experiment::LogVI_EdcaTxopNDequeueAction (Ptr<WifiMacQueueItem const> item)
{
	Time waited = Now() - item->GetTimeStamp();

	NS_LOG_INFO("VI_EdcaTxopN Queue Dequeue action at " << Now().GetSeconds()
			<< " Item " << item->GetPacket()->GetUid()
			<< " waited for " << (Now() - item->GetTimeStamp()).GetMicroSeconds() << "us"
			<< std::endl);

//	m_queueWaitRecord["VI_EdcaTxopN"] = std::pair<Time, uint64_t>(m_queueWaitRecord["VI_EdcaTxopN"].first+waited, m_queueWaitRecord["VI_EdcaTxopN"].second+1);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(m_queueWaitRecord[0].first+waited, m_queueWaitRecord[0].second+1);

}
void
Experiment::LogBE_EdcaTxopNEnqueueAction (Ptr<WifiMacQueueItem const> item)
{
//	NS_LOG_INFO("BE_EdcaTxopN Queue Enqueue action Item "<< item->GetPacket()->GetUid()
//			<< " at " << Now().GetSeconds()
//			<< std::endl);
}

void
Experiment::LogBE_EdcaTxopNDequeueAction (Ptr<WifiMacQueueItem const> item)
{
	Time waited = Now() - item->GetTimeStamp();

//	NS_LOG_INFO("BE_EdcaTxopN Queue Dequeue action at " << Now().GetSeconds()
//			<< " Item " << item->GetPacket()->GetUid()
//			<< " waited for " << (Now() - item->GetTimeStamp()).GetMicroSeconds() << "us"
//			<< std::endl);

//	m_queueWaitRecord["BE_EdcaTxopN"] = std::pair<Time, uint64_t>(m_queueWaitRecord["BE_EdcaTxopN"].first+waited, m_queueWaitRecord["BE_EdcaTxopN"].second+1);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(m_queueWaitRecord[0].first+waited, m_queueWaitRecord[0].second+1);

}
void
Experiment::LogBK_EdcaTxopNEnqueueAction (Ptr<WifiMacQueueItem const> item)
{
	NS_LOG_INFO("BK_EdcaTxopN Queue Enqueue action Item "<< item->GetPacket()->GetUid()
			<< " at " << Now().GetSeconds()
			<< std::endl);
}

void
Experiment::LogBK_EdcaTxopNDequeueAction (Ptr<WifiMacQueueItem const> item)
{
	Time waited = Now() - item->GetTimeStamp();

	NS_LOG_INFO("BK_EdcaTxopN Queue Dequeue action" << " Item " << item->GetPacket()->GetUid()
			<<" at " << Now().GetSeconds()
			<< " waited for " << (Now() - item->GetTimeStamp()).GetMicroSeconds() << "us"
			<< std::endl);

//	m_queueWaitRecord["BK_EdcaTxopN"] = std::pair<Time, uint64_t>(m_queueWaitRecord["BE_EdcaTxopN"].first+waited, m_queueWaitRecord["BE_EdcaTxopN"].second+1);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(m_queueWaitRecord[0].first+waited, m_queueWaitRecord[0].second+1);

}

void
Experiment::SetupQueueMonitoring(Ptr<ApWifiMac> mac)
{
	PointerValue ptr;
	Ptr<WifiMacQueue> wifiMacQueue;

	mac->GetAttribute("DcaTxop", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<DcaTxop>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&Experiment::LogDcaTxopEnqueueAction, this));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&Experiment::LogDcaTxopDequeueAction, this));
//	m_queueWaitRecord["DcaTxop"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(Seconds(0),0);


	mac->GetAttribute("BK_EdcaTxopN", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<EdcaTxopN>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&Experiment::LogBK_EdcaTxopNEnqueueAction, this));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&Experiment::LogBK_EdcaTxopNDequeueAction, this));
//	m_queueWaitRecord["BK_EdcaTxopN"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(Seconds(0),0);


	mac->GetAttribute("VI_EdcaTxopN", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<EdcaTxopN>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&Experiment::LogVI_EdcaTxopNEnqueueAction, this));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&Experiment::LogVI_EdcaTxopNDequeueAction, this));
//	m_queueWaitRecord["VI_EdcaTxopN"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(Seconds(0),0);


	mac->GetAttribute("VO_EdcaTxopN", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<EdcaTxopN>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&Experiment::LogVO_EdcaTxopNEnqueueAction, this));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&Experiment::LogVO_EdcaTxopNDequeueAction, this));
//	m_queueWaitRecord["VO_EdcaTxopN"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(Seconds(0),0);


	mac->GetAttribute("BE_EdcaTxopN", ptr);
	wifiMacQueue = ptr.GetObject()->GetObject<EdcaTxopN>()->GetQueue();
	wifiMacQueue->TraceConnectWithoutContext("Enqueue", MakeCallback(&Experiment::LogBE_EdcaTxopNEnqueueAction, this));
	wifiMacQueue->TraceConnectWithoutContext("Dequeue", MakeCallback(&Experiment::LogBE_EdcaTxopNDequeueAction, this));
//	m_queueWaitRecord["BE_EdcaTxopN"] = std::pair<Time, uint64_t>(Seconds(0),0);
	m_queueWaitRecord[0] = std::pair<Time, uint64_t>(Seconds(0),0);

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
		Ptr<ApWifiMac> mac = device.Get(0)->GetObject<WifiNetDevice>()->GetMac()->GetObject<ApWifiMac>();
		SetupQueueMonitoring(mac);

//		Ptr<WifiPhy> phy = device.Get(0)->GetObject<WifiNetDevice>()->GetPhy();
//		phy->TraceConnectWithoutContext("MonitorSnifferTx", MakeCallback(&Experiment::MonitorPhyTx, this));

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

	for(NetDeviceContainer::Iterator it = m_relayClusterDevice.Begin(); it!= m_relayClusterDevice.End(); it++)
	{
		uint32_t id = (*it)->GetNode()->GetId();

		ClusterSleep(id, Now());

//		if(id==7)
//		{
//			Ptr<ApWifiMac> mac = (*it)->GetObject<WifiNetDevice>()->GetMac()->GetObject<ApWifiMac>();
//			mac->SetAttribute("BeaconGeneration", BooleanValue(true));
//			break;
//
//		}
//		double resRate = m_relayResourceMap[id];
//
//		ClusterSleep(id, Seconds(0));
//
//		ClusterWakeup(id, Seconds(m_usedResources+20));
//		ClusterSleep(id, Seconds(m_usedResources+20 + resRate));
//
//		NetDeviceContainer src = NetDeviceContainer(*it);
//		Ptr<Node> node = (*it)->GetNode();
//		NetDeviceContainer dst = m_clusterDevices[node->GetId()];
//		InstallApplications(src, dst, Seconds(m_usedResources+40), Seconds(m_usedResources+20 + resRate));
//
//		m_usedResources += resRate;
	}

//	for(NetDeviceContainer::Iterator it = m_relayClusterDevice.Begin(); it!= m_relayClusterDevice.End(); it++)
//	{
//		double resRate = m_relayResourceMap[id];
//		double duration = m_totalResources*resRate/100;
//
//
//		Ptr<UniformRandomVariable> rand = CreateObject<UniformRandomVariable>();
//		Ptr<WifiPhy> staPhy = DynamicCast<WifiNetDevice> (*it)->GetPhy ();
//
//		Simulator::Schedule (Seconds(rand->GetValue(0,10)),
//				&WifiPhy::ResumeFromSleep, staPhy);
//
//		NetDeviceContainer src = NetDeviceContainer(*it);
//		Ptr<Node> node = (*it)->GetNode();
//		NetDeviceContainer dst = m_clusterDevices[node->GetId()];
//		double timeStart = m_remainingResource - duration + (g_simTime - m_totalResources);
//		double timeStop = m_remainingResource + (g_simTime - m_totalResources);
//		InstallApplications(src, dst, Seconds(timeStart), Seconds(timeStop));
//
//		m_remainingResource -= duration;
//	}


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
//	sink->BindToNetDevice(device);
	sink->SetRecvCallback (MakeCallback ( &Experiment::ReceivePacket, this));

//	m_packetsTotal[device] = 0;
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
		m_packetsTotal[device] += packet->GetSize();
	}
}

//TODO This install an app sending traffic from every netdevice in src to every netdevice in dst
ApplicationContainer
Experiment::InstallApplications (NetDeviceContainer src, NetDeviceContainer dst, Time start, Time stop)
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
		Ipv4InterfaceAddress addr_src = ipv4_src->GetInterface(ipv4_src->GetInterfaceForDevice(*it1))->GetAddress(0);

		OnOffHelper onoff ("ns3::UdpSocketFactory",
				Address (InetSocketAddress (addr_dst.GetLocal(), port))); // TODO Send traffic to AP?
		onoff.SetAttribute ("OnTime", StringValue ("ns3::ConstantRandomVariable[Constant=1]"));
		onoff.SetAttribute ("OffTime", StringValue ("ns3::ConstantRandomVariable[Constant=0]"));
		onoff.SetAttribute ("PacketSize", UintegerValue(1472)); // Total IP packet size of 1500
		onoff.SetConstantRate(DataRate("1Gbps"));
		apps.Add(onoff.Install((*it1)->GetNode()));

		NS_LOG_INFO("address " << addr_src.GetLocal() << " will send packets to address " << addr_dst.GetLocal()
				<< " From " << start.GetSeconds() << "s to " << stop.GetSeconds() << " for " << (stop - start).GetSeconds()
				<< "s" << std::endl);

	      }
	}

	apps.Start (start);
	apps.Stop (stop);

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
		onoff.SetConstantRate(DataRate("1Gbps"));
		apps.Add(onoff.Install((*it)->GetNode()));

//		NS_LOG_INFO("address " << addr_src.GetLocal() << " will send packets to address " << addr_dst.GetLocal()
//				<< " From " << start.GetSeconds() << " to " << stop.GetSeconds() << " for " << (stop - start).GetSeconds()
//				<< std::endl);
	}

	return apps;
}

void
Experiment::MonitorPhyTx (Ptr<const Packet> packet, uint16_t channelFreqMhz, WifiTxVector txVector, MpduInfo aMpdu)
{
	m_totalPhyTxBytes += packet->GetSize();
}

void
EnableBroadcast(Ptr<Application> app)
{
	Ptr<OnOffApplication> onoffApp = DynamicCast<OnOffApplication>(app);
    Ptr<Socket> socket = onoffApp->GetSocket();
    socket->SetAllowBroadcast(true);
}

ApplicationContainer
Experiment::InstallApplications (NetDeviceContainer src, Ipv4Address address, Time start, Time stop)
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

	apps.Start (start);
	apps.Stop (stop);

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

	m_totalPhyTxBytes = 0;
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
	m_totalPhyTxBytes = 0;
	m_packetsTotal.clear();
	m_queueWaitRecord.clear();
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
Experiment::SetupHooks (uint32_t id)
{
	NetDeviceContainer devices = m_clusterDevices[id];
	for(uint32_t i = 0; i < devices.GetN(); i++)
	{
		Ptr<StaWifiMac> mac = devices.Get(i)->GetObject<WifiNetDevice> ()->GetMac()->GetObject<StaWifiMac> ();
//		UintegerValue oldVal, newVal;
//		mac->GetAttribute("MaxMissedBeacons", oldVal);
		mac->SetAttribute("MaxMissedBeacons", UintegerValue(1000));
//		mac->GetAttribute("MaxMissedBeacons", newVal);
//		std::cout<< "MaxMissedBeacons was " << oldVal.Get() << " and became " << newVal.Get() << " for client " << GetIpv4OfMac48(mac->GetAddress()).GetLocal() << std::endl;
//
		mac->TraceConnectWithoutContext("DeAssoc", MakeCallback(&Experiment::LogDeAssoc, this));
		mac->TraceConnectWithoutContext("Assoc", MakeCallback(&Experiment::LogAssoc, this));

//		EnableActiveProbing (mac);

		SetupReceivePacket(devices.Get(i));
	}
}

void
Experiment::Run(int argc, char *argv[])
{
	std::cout.flush();

//	long int time = static_cast<long int>(std::time(nullptr));

//	std::string animFile = "animation-";
//	animFile.append(std::to_string(time).c_str());
//	animFile.append(".xml");

	CommandLine cmd;
	cmd.AddValue("simTime","Simulation Time", g_simTime);
//	cmd.AddValue("animFile","Animation xml filename", animFile);

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

		SetupHooks (id);
		ClusterWakeup(id, Now());
//		ApplicationContainer apps = InstallApplications(id);
		while(!ClientsAssociated(id))
		{
//			apps.Start(Now());
//			apps.Stop(Now() + Seconds(10));
			Simulator::Stop(Seconds(10));
			Simulator::Run();
		}
		ResetStats();
		NS_LOG_UNCOND("Downlink traffic started");
		Simulator::Stop(Seconds(resRate));

		ApplicationContainer apps = InstallApplications(id);
		apps.Start(Seconds(0));
		apps.Stop(Seconds(resRate));


		Simulator::Run();
		ClusterSleep(id, Now());


		Ptr<NetDevice> device;
		Ipv4InterfaceAddress address;
		std::cout << "Relay ID\t\tIP Address\t\tAvg Queue Wait";
		std::cout.flush();
		NetDeviceContainer devices = m_clusterDevices[id];
		for(uint32_t i = 0; i < devices.GetN(); i++)
		{
			device = devices.Get(i);
			address = device->GetNode()->GetObject<Ipv4>()->GetAddress(1, 0);
			std::cout << "\t\tThroughput for " << address.GetLocal();
			std::cout.flush();
		}
		device = *rit;
		Ptr<Ipv4> ipv4 = device->GetNode()->GetObject<Ipv4>();
		address = ipv4->GetAddress(ipv4->GetInterfaceForDevice(device), 0);


		std::cout << std::endl;
		std::cout << id << "\t\t\t" << address.GetLocal() << "\t\t";
		std::cout.flush();

		std::pair<Time, uint64_t> pair = m_queueWaitRecord[0];
		double queueAvgWait = pair.first.GetSeconds();
		double queueTotalItems = pair.second;
		std::cout << (queueTotalItems == 0 ? 0:queueAvgWait/queueTotalItems) <<"s";
		std::cout.flush();

		for(uint32_t i = 0; i < devices.GetN(); i++)
		{
			device = devices.Get(i);
			double throughput = m_packetsTotal[device]/resRate/1.0e6;
			std::cout << "\t\t\t" << throughput << "MBytes/s";
			std::cout.flush();
		}
		std::cout << std::endl;

//		std::cout << "Total cluster downlink Throughput " << m_totalPhyTxBytes/resRate/1e6 << " MBytes/s" << std::endl;

//		for(std::map<std::string, std::pair<Time, uint64_t> >::iterator it = m_queueWaitRecord.begin(); it != m_queueWaitRecord.end(); it++)
//		{
//			std::string queueType = it->first;
//			double queueAvgWait = it->second.first.GetMicroSeconds();
//			double queueTotalItems = it->second.second;
//			if(queueTotalItems)
//			{
//				double avgWait = queueAvgWait/queueTotalItems;
//				std::cout << it->first.c_str() << "Queue average wait time is " << avgWait <<"us" << std::endl;
//			}
//		}

		ResetStats();

		std::cout << "--- Finished Running Downlink at Cluster " << id << " ---";
		std::cout.flush();

	}





	Simulator::Destroy();

	//  std::cout << experiment.GetPacketsTotal().size() << std::endl;

}

