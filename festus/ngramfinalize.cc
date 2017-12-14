// festus/ngramfinalize.cc
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Copyright 2016 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Makes all states in an n-gram model final with their correct final weights
// computed along the backoff path. This makes n-gram models usable with older
// versions of OpenFst (<1.5.0).

#include <cstring>
#include <memory>

#include <fst/compat.h>
#include <fst/fstlib.h>
#include <fst/extensions/ngram/ngram-fst.h>

namespace festus {

// OpenFst < 1.5.0 lacks the method fst::Matcher::Final(), so it cannot
// correctly compute the true final weight (via backoff) of states that don't
// have an explicit final weight. To work around this, compute all final
// weights ahead of time: for every non-final state, follow its backoff path to
// a final state and multiply the weights along the backoff path.
template <class Arc>
bool MakeAllStatesFinal(fst::MutableFst<Arc> *fst,
                        typename Arc::Label phi_label) {
  typedef typename Arc::StateId StateId;
  typedef typename Arc::Weight Weight;
  for (StateId s = 0; s < fst->NumStates(); ++s) {
    if (fst->Final(s) != Weight::Zero()) continue;
    // Pull final costs up to non-final states along backoff path.
    Weight weight = Weight::One();
    StateId state = s;
    fst::Matcher<fst::MutableFst<Arc>> matcher(*fst, fst::MATCH_INPUT);
    while (fst->Final(state) == Weight::Zero()) {
      matcher.SetState(state);
      if (!matcher.Find(phi_label == 0 ? -1 : phi_label)) {
        LOG(ERROR) << "No backoff arc leaving non-final state " << state;
        return false;
      }
      bool found = false;
      for (; !matcher.Done(); matcher.Next()) {
        const Arc &arc = matcher.Value();
        if (arc.ilabel == fst::kNoLabel) {
          VLOG(3) << "Arc has kNoLabel as ilabel. Ignoring arc at state "
                  << state << ": " << arc.ilabel << ", " << arc.olabel
                  << ", " << arc.weight.Value() << ", " << arc.nextstate;
          continue;
        }
        if (found) {
          LOG(WARNING) << "Backoff arc already found! Ignoring arc at state "
                       << state << ": " << arc.ilabel << ", " << arc.olabel
                       << ", " << arc.weight.Value() << ", " << arc.nextstate;
          continue;
        }
        found = true;
        if (arc.nextstate == state) {
          LOG(ERROR) << "Backoff self-loop detected at state " << state;
          return false;
        }
        state = arc.nextstate;
        weight = Times(weight, arc.weight);
      }
    }
    weight = Times(weight, fst->Final(state));
    fst->SetFinal(s, weight);
    VLOG(1) << "Final weight of state " << s << " set to " << weight.Value();
  }
  return true;
}

}  // namespace festus

static const char kUsage[] =
    R"(Makes all states in an n-gram model final with their correct final
weights computed along the backoff path. This makes n-gram models usable with
older versions of OpenFst (<1.5.0).

Usage:
  ngramfinalize [--flags] [in.fst [out.fst]]
)";

DEFINE_int32(phi_label, 0, "Phi (failure, backoff) label");

DEFINE_bool(to_runtime_model, false, "Convert to the runtime model format");

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc > 3) {
    ShowUsage();
    return 2;
  }

  string in_name = (argc > 1 && std::strcmp(argv[1], "-") != 0) ? argv[1] : "";
  string out_name = (argc > 2 && std::strcmp(argv[2], "-") != 0) ? argv[2] : "";

  std::unique_ptr<fst::StdVectorFst> model(fst::StdVectorFst::Read(in_name));
  if (!model) return 2;

  if (!festus::MakeAllStatesFinal(model.get(), FLAGS_phi_label)) return 2;

  if (FLAGS_to_runtime_model) {
    fst::VectorFst<fst::LogArc> log_fst;
    fst::Map(*model, &log_fst, fst::StdToLogMapper());
    log_fst.SetInputSymbols(nullptr);
    log_fst.SetOutputSymbols(nullptr);
    fst::NGramFst<fst::LogArc> ngram(log_fst);
    ngram.Write(out_name);
  } else {
    model->Write(out_name);
  }

  return 0;
}
