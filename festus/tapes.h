// festus/iterator.h
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
// Copyright 2018 Google LLC
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Classes that represent FST tapes, for use in template metaprogramming.

#ifndef FESTUS_TAPES_H__
#define FESTUS_TAPES_H__

#include <utility>

#include <fst/arcsort.h>
#include <fst/epsnormalize.h>
#include <fst/fst.h>
#include <fst/project.h>
#include <fst/replace.h>

namespace festus {

struct NeitherTape {
  static constexpr fst::MatchType kMatch = fst::MATCH_NONE;

  static constexpr fst::ReplaceLabelType kReplaceLabel =
      fst::REPLACE_LABEL_NEITHER;
};

struct InputTape {
  template <class A> using Compare = fst::ILabelCompare<A>;

  static constexpr fst::EpsNormalizeType kEpsNormalize = fst::EPS_NORM_INPUT;

  static constexpr fst::MatchType kMatch = fst::MATCH_INPUT;

  static constexpr fst::ProjectType kProject = fst::PROJECT_INPUT;

  static constexpr fst::ReplaceLabelType kReplaceLabel =
      fst::REPLACE_LABEL_INPUT;

  template <class Arc>
  static typename Arc::Label GetLabel(const Arc &arc) { return arc.ilabel; }

  template <class FST>
  static const fst::SymbolTable *GetSymbols(const FST &fst) {
    return fst.InputSymbols();
  }

  template <class FST>
  static void SetSymbols(FST *fst, const fst::SymbolTable *symbols) {
    fst->SetInputSymbols(symbols);
  }
};

struct OutputTape {
  template <class A> using Compare = fst::OLabelCompare<A>;

  static constexpr fst::EpsNormalizeType kEpsNormalize = fst::EPS_NORM_OUTPUT;

  static constexpr fst::MatchType kMatch = fst::MATCH_OUTPUT;

  static constexpr fst::ProjectType kProject = fst::PROJECT_OUTPUT;

  static constexpr fst::ReplaceLabelType kReplaceLabel =
      fst::REPLACE_LABEL_OUTPUT;

  template <class Arc>
  static typename Arc::Label GetLabel(const Arc &arc) { return arc.olabel; }

  template <class FST>
  static const fst::SymbolTable *GetSymbols(const FST &fst) {
    return fst.OutputSymbols();
  }

  template <class FST>
  static void SetSymbols(FST *fst, const fst::SymbolTable *symbols) {
    fst->SetOutputSymbols(symbols);
  }
};

struct BothTapes {
  using Pair = std::pair<const fst::SymbolTable *, const fst::SymbolTable *>;

  static constexpr fst::MatchType kMatch = fst::MATCH_BOTH;

  static constexpr fst::ReplaceLabelType kReplaceLabel =
      fst::REPLACE_LABEL_BOTH;

  template <class FST>
  static Pair GetSymbols(const FST &fst) {
    return std::make_pair(fst.InputSymbols(), fst.OutputSymbols());
  }

  template <class FST>
  static void SetSymbols(FST *fst, Pair pair) {
    fst->SetInputSymbols(pair.first);
    fst->SetOutputSymbols(pair.second);
  }
};

template <class Tape, class Arc>
void ArcSort(fst::MutableFst<Arc> *fst) {
  typename Tape::template Compare<Arc> comp;
  fst::ArcSort<Arc>(fst, comp);
}

template <class Tape, class Arc>
void EpsNormalize(const fst::Fst<Arc> &ifst, fst::MutableFst<Arc> *ofst) {
  fst::EpsNormalize<Arc>(ifst, ofst, Tape::kEpsNormalize);
}

template <class Tape, class Arc>
void Project(fst::MutableFst<Arc> *fst) {
  fst::Project<Arc>(fst, Tape::kProject);
}

template <class Tape, class F>
fst::SortedMatcher<F> SortedMatcher(
    const F &fst,
    typename F::Arc::Label binary_label = 1) {
  return fst::SortedMatcher<F>(fst, Tape::kMatch, binary_label);
}

template <class Tape, class Arc>
typename Arc::Label GetLabel(const Arc &arc) { return Tape::GetLabel(arc); }

template <class Tape, class FST>
auto GetSymbols(const FST &fst) -> decltype(Tape::GetSymbols(fst)) {
  return Tape::GetSymbols(fst);
}

template <class Tape, class FST, class Symbols>
void SetSymbols(FST *fst, Symbols symbols) {
  Tape::SetSymbols(fst, symbols);
}

}  // namespace festus

#endif  // FESTUS_TAPES_H__
