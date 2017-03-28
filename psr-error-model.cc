#include "psr-error-model.h"
#include "ns3/core-module.h"

using namespace ns3;

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
	return m_rate; //TODO too simple, maybe needs some testing
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

