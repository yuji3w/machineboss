#ifndef DPMATRIX_INCLUDED
#define DPMATRIX_INCLUDED

#include "eval.h"
#include "seqpair.h"
#include "logsumexp.h"

class DPMatrix {
public:
  typedef long InputIndex;
  typedef long OutputIndex;

private:
  typedef long long CellIndex;

  vguard<double> cellStorage;

  inline CellIndex nCells() const {
    return nStates * (inLen + 1) * (outLen + 1);
  }

  inline CellIndex cellIndex (InputIndex inPos, OutputIndex outPos, StateIndex state) const {
    return (inPos * (outLen + 1) + outPos) * nStates + state;
  }

protected:
  inline void accumulate (double& ll, const EvaluatedMachineState::InOutTransMap& inOutTransMap, InputToken inTok, OutputToken outTok, InputIndex inPos, OutputIndex outPos, function<double(double,double)> reduce) const {
    auto visit = [&] (EvaluatedMachineState::TransIndex, double t) { ll = reduce(ll,t); };
    iterate (inOutTransMap, inTok, outTok, inPos, outPos, visit);
  }

  inline void accumulateCounts (double logOddsRatio, vguard<double>& transCounts, const EvaluatedMachineState::InOutTransMap& inOutTransMap, InputToken inTok, OutputToken outTok, InputIndex inPos, OutputIndex outPos) const {
    auto visit = [&] (EvaluatedMachineState::TransIndex ti, double tll) { transCounts[ti] += exp (logOddsRatio + tll); };
    iterate (inOutTransMap, inTok, outTok, inPos, outPos, visit);
  }

  inline void iterate (const EvaluatedMachineState::InOutTransMap& inOutTransMap, InputToken inTok, OutputToken outTok, InputIndex inPos, OutputIndex outPos, function<void(EvaluatedMachineState::TransIndex,double)> visit) const {
    if (inOutTransMap.count (inTok)) {
      const EvaluatedMachineState::OutTransMap& outTransMap = inOutTransMap.at (inTok);
      if (outTransMap.count (outTok)) {
	const EvaluatedMachineState::Trans& trans = outTransMap.at (outTok);
	visit (trans.transIndex, cell(inPos,outPos,trans.state) + trans.logWeight);
      }
    }
  }

  static inline double sum_reduce (double x, double y) { return log_sum_exp(x,y); }
  static inline double max_reduce (double x, double y) { return max(x,y); }

public:
  const EvaluatedMachine& machine;
  const SeqPair& seqPair;
  const vguard<InputToken> input;
  const vguard<OutputToken> output;
  const InputIndex inLen;
  const OutputIndex outLen;
  const StateIndex nStates;

  DPMatrix (const EvaluatedMachine& machine, const SeqPair& seqPair);

  void writeJson (ostream& out) const;
  
  inline double& cell (InputIndex inPos, OutputIndex outPos, StateIndex state) { return cellStorage[cellIndex(inPos,outPos,state)]; }
  inline const double cell (InputIndex inPos, OutputIndex outPos, StateIndex state) const { return cellStorage[cellIndex(inPos,outPos,state)]; }
};

#endif /* DPMATRIX_INCLUDED */
