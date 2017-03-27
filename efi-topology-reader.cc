#include "efi-topology-reader.h"
#include "ns3/core-module.h"

using namespace ns3;
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

	std::istringstream lineBuffer1; // First line for Node parameters
	std::istringstream lineBuffer2; // Seconds line for Resources Allocation parameters

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
