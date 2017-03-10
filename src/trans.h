#ifndef TRANSDUCER_INCLUDED
#define TRANSDUCER_INCLUDED

#include <string>
#include <map>
#include <set>
#include <list>
#include <json.hpp>
#include "weight.h"
#include "vguard.h"

using namespace std;
using json = nlohmann::json;

typedef unsigned long long StateIndex;

#define MachineWaitTag "wait"
#define MachineSilentTag "silent"

typedef string OutputSymbol;
typedef string InputSymbol;
typedef json StateName;

struct MachineTransition {
  InputSymbol in;
  OutputSymbol out;
  StateIndex dest;
  TransWeight weight;
  MachineTransition();
  MachineTransition (InputSymbol, OutputSymbol, StateIndex, TransWeight);
  bool inputEmpty() const;
  bool outputEmpty() const;
  bool isSilent() const;  // inputEmpty() && outputEmpty()
  bool isLoud() const;  // !isSilent()
};
typedef list<MachineTransition> TransList;

struct MachineState {
  StateName name;
  TransList trans;
  MachineState();
  bool exitsWithInput() const;  // true if this has an input transition
  bool exitsWithoutInput() const;  // true if this has a non-input transition
  bool exitsWithIO() const;  // true if this has any transitions with input and/or output
  bool exitsWithoutIO() const;  // true if this has any transitions without input or output
  bool terminates() const;  // true if this has no outgoing transitions. Note that the end state is not required to have this property
  bool waits() const;  // !exitsWithoutInput()
  bool continues() const;  // !exitsWithInput() && !terminates()
  bool isSilent() const;  // !exitsWithIO()
  bool isLoud() const;  // exitsWithIO() && !exitsWithoutIO()
};

struct Machine {
  vguard<MachineState> state;
  
  Machine();
  StateIndex nStates() const;
  size_t nTransitions() const;
  StateIndex startState() const;
  StateIndex endState() const;

  void writeJson (ostream& out) const;
  string toJsonString() const;
  void readJson (istream& in);
  void readJson (const json& json);
  static Machine fromJson (istream& in);
  static Machine fromFile (const char* filename);

  vguard<InputSymbol> inputAlphabet() const;
  vguard<OutputSymbol> outputAlphabet() const;

  set<StateIndex> accessibleStates() const;

  static Machine compose (const Machine& first, const Machine& second);

  bool isErgodicMachine() const;  // all states accessible
  bool isWaitingMachine() const;  // all states wait or continue
  bool isAdvancingMachine() const;  // no silent i->j transitions where j<i

  Machine ergodicMachine() const;  // remove unreachable states
  Machine waitingMachine() const;  // convert to waiting machine
  Machine advancingMachine() const;  // convert to advancing machine
};

struct TransAccumulator {
  map<StateIndex,map<InputSymbol,map<OutputSymbol,TransWeight> > > t;
  void accumulate (InputSymbol in, OutputSymbol out, StateIndex dest, TransWeight w);
  TransList transitions() const;
};

#endif /* TRANSDUCER_INCLUDED */
