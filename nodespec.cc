#include "nodespec.h"
#include "ns3/log.h"


using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("nodespec");

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

uint32_t
NodeSpec::GetId ()
{
	return m_id;
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
