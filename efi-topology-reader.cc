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

//		std::cout << apNodeSpec;

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

		int i =0;
		double value = 0;

		NodeSpec nodeSpec;
		while(lineBuffer1 >> value)
		{
			switch(i)
			{
			case 0:
				id = (uint32_t)value;
				break;
			case 1:
				type = (uint32_t)value;
				if(type == 1)
					lineBuffer2 >> resRate;
				NS_ASSERT(resRate >= 0);
				break;
			case 2:
				locX = value;
				break;
			case 3:
				locY = value;
				break;
			case 4:
				oPsr = value;
				NS_ASSERT(oPsr >= 0);
				break;
			case 5:
				rPsr = value;
				NS_ASSERT(rPsr >= 0);
				break;
			case 6:
				nPsr = value;
				NS_ASSERT(nPsr >= 0);
				break;
			case 7:
			  if(type==3 || type==1)
			    relayId=0;
			  else
			    relayId = (uint32_t) value;
				break;
			default:
				NS_FATAL_ERROR("Can't reach 8 parameters");

			}
			i++;

			if(i==8)
			{
				if(type == 1 || type == 3)
					relayId = 0;

				nodeSpec = NodeSpec(id, (NodeSpec::NodeType)type, Vector3D(locX,locY,0), relayId, oPsr*0.01, rPsr*0.01, nPsr*0.01, resRate);
				nodeSpecs.push_back(nodeSpec);
				i = 0;
			}
		}
		lineBuffer2 >> resRate;

		NodeSpec apNodeSpec(0, NodeSpec::AP, Vector3D(0, 0, 0), relayId, 1, 1, 1, resRate);
		nodeSpecs.insert(nodeSpecs.begin(), apNodeSpec);

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
