// festus/alignables-util.h
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
// Copyright 2015, 2016 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Utility class for working with latent alignment models over chunks/phrases.

#ifndef FESTUS_ALIGNABLES_UTIL_H__
#define FESTUS_ALIGNABLES_UTIL_H__

#include <memory>

#include <fst/arc.h>
#include <fst/arcsort.h>
#include <fst/compact-fst.h>
#include <fst/compose.h>
#include <fst/determinize.h>
#include <fst/fst.h>
#include <fst/minimize.h>
#include <fst/project.h>
#include <fst/rmepsilon.h>
#include <fst/symbol-table.h>
#include <fst/vector-fst.h>

#include "festus/alignables.pb.h"
#include "festus/label-maker.h"

namespace festus {

// TODO: move into fst-util.h
template <class A>
using CompactStringFst = fst::CompactFst<A, fst::StringCompactor<A>>;

// TODO: merge into label-maker.h?
template <class A>
CompactStringFst<A> MakeStringFst(
    const string &str,
    const LabelMaker &label_maker) {
  // Work around missing methods CompactFst::Set{In,Out}putSymbols():
  fst::VectorFst<A> empty;
  empty.SetInputSymbols(label_maker.Symbols());
  empty.SetOutputSymbols(label_maker.Symbols());
  CompactStringFst<A> string_fst(empty);
  LabelMaker::Labels labels;
  if (label_maker.StringToLabels(str, &labels)) {
    string_fst.SetCompactElements(labels.begin(), labels.end());
  }
  return string_fst;
}

template <class A>
fst::VectorFst<A> ProjectIntoPairLattice(
    const fst::Fst<A> &fst,
    const fst::Fst<A> &inverse_projection,
    bool optimize=false) {
  // TODO: The current implementation is too conservative.
  fst::VectorFst<A> lattice;
  // Here we don't connect during composition, deferring it until RmEpsilon:
  fst::Compose(fst, inverse_projection, &lattice, fst::ComposeOptions(false));
  fst::Project(&lattice, fst::PROJECT_OUTPUT);
  lattice.Properties(fst::kAcyclic | fst::kUnweighted, true);
  fst::RmEpsilon(&lattice);
  if (optimize) {
    fst::VectorFst<A> opt;
    fst::Determinize(lattice, &opt);
    opt.Properties(fst::kAcyclic | fst::kUnweighted, true);
    fst::AcceptorMinimize(&opt);
    fst::TopSort(&opt);
    lattice = opt;
  }
  fst::ArcSort(&lattice, fst::OLabelCompare<A>());
  lattice.Properties(fst::kFstProperties, true);
  return lattice;
}

class AlignablesUtil {
 public:
  typedef fst::Log64Arc Arc;

  static string MakePairSymbol(const Alignable &alignable);

  static std::unique_ptr<AlignablesUtil> New(const AlignablesSpec &spec);

  CompactStringFst<Arc> MakeInputFst(const string &input) const {
    return MakeStringFst<Arc>(input, *input_label_maker_);
  }

  CompactStringFst<Arc> MakeOutputFst(const string &output) const {
    return MakeStringFst<Arc>(output, *output_label_maker_);
  }

  fst::VectorFst<Arc> MakePairLatticeForInputFst(
      const fst::Fst<Arc> &input_fst) const {
    return ProjectIntoPairLattice(input_fst, input_to_pair_fst_);
  }

  fst::VectorFst<Arc> MakePairLatticeForOutputFst(
      const fst::Fst<Arc> &output_fst) const {
    return ProjectIntoPairLattice(output_fst, output_to_pair_fst_);
  }

  fst::VectorFst<Arc> MakePairLatticeForInput(const string &input) const {
    return MakePairLatticeForInputFst(MakeInputFst(input));
  }

  fst::VectorFst<Arc> MakePairLatticeForOutput(const string &output) const {
    return MakePairLatticeForOutputFst(MakeOutputFst(output));
  }

  const fst::SymbolTable *InputSymbols() const {
    return input_label_maker_->Symbols();
  }

  const fst::SymbolTable *OutputSymbols() const {
    return output_label_maker_->Symbols();
  }

  const fst::SymbolTable *PairSymbols() const {
    return pair_fsa_.InputSymbols();
  }

  const fst::VectorFst<Arc> &InputToPairFst() const {
    return input_to_pair_fst_;
  }

  const fst::VectorFst<Arc> &OutputToPairFst() const {
    return output_to_pair_fst_;
  }

  const fst::VectorFst<Arc> &PairFsa() const {
    return pair_fsa_;
  }

  const fst::VectorFst<Arc> &PairToInputFst() const {
    return pair_to_input_fst_;
  }

  const fst::VectorFst<Arc> &PairToOutputFst() const {
    return pair_to_output_fst_;
  }

  void RemoveForbiddenFactors(fst::VectorFst<Arc> *pair_fsa) const;

 private:
  AlignablesUtil() = default;

  bool Init(const AlignablesSpec &spec);

  std::unique_ptr<const LabelMaker> input_label_maker_;
  std::unique_ptr<const LabelMaker> output_label_maker_;

  // Product space FSA, essentially an encoded transducer.
  fst::VectorFst<Arc> pair_fsa_;

  // Inverse canonical projections.
  fst::VectorFst<Arc> input_to_pair_fst_;
  fst::VectorFst<Arc> output_to_pair_fst_;

  // Canonical projections.
  fst::VectorFst<Arc> pair_to_input_fst_;
  fst::VectorFst<Arc> pair_to_output_fst_;

  fst::VectorFst<Arc> forbidden_factors_fsa_;
};

}  // namespace festus

#endif  // FESTUS_ALIGNABLES_UTIL_H__
