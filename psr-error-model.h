
#include "ns3/error-rate-model.h"
#include "ns3/wifi-module.h"
#include "ns3/type-id.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {
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

}
