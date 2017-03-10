#ifndef COUNTS_INCLUDED
#define COUNTS_INCLUDED

#include "eval.h"
#include "seqpair.h"
#include "constraints.h"

// E-step
struct MachineCounts {
  vguard<vguard<double> > count;  // indexed: count[state][nTrans]
  MachineCounts (const EvaluatedMachine&, const SeqPair&);
  MachineCounts& operator+= (const MachineCounts&);
};

// M-step
struct MachineLagrangian {
  vguard<string> param, lagrangeMultiplier;
  TransWeight lagrangian;
  MachineLagrangian (const Machine&, const MachineCounts&, const Constraints&);
  Params optimize (const Params& seed) const;
};

#endif /* COUNTS_INCLUDED */

