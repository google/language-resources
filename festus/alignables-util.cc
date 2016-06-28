// festus/alignables-util.cc
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

#include "festus/alignables-util.h"

#include <cstddef>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include <fst/arc-map.h>
#include <fst/arcsort.h>
#include <fst/compat.h>
#include <fst/concat.h>
#include <fst/determinize.h>
#include <fst/difference.h>
#include <fst/fst.h>
#include <fst/minimize.h>
#include <fst/rmepsilon.h>
#include <fst/symbol-table.h>
#include <fst/vector-fst.h>

#include "festus/alignables.pb.h"
#include "festus/label-maker.h"
#include "festus/proto-util.h"

using fst::SymbolTable;
using fst::VectorFst;

namespace festus {
namespace {

const char kEpsilonSymbol[] = "<epsilon>";

inline AlignablesUtil::Arc::StateId AddStartFinal(
    VectorFst<AlignablesUtil::Arc> *fst) {
  auto start = fst->AddState();
  fst->SetStart(start);
  fst->SetFinal(start, AlignablesUtil::Arc::Weight::One());
  return start;
}

template <class A>
class PrefixTree {
 public:
  typedef typename A::Label Label;
  typedef typename A::StateId StateId;
  typedef typename A::Weight Weight;

  explicit PrefixTree(VectorFst<A> *fst) : fst_(fst) {
    AddStartFinal(fst_);
  }

  void Add(const std::vector<Label> &ilabels, Label olabel) {
    DCHECK_GT(olabel, 0);
    if (ilabels.empty()) {
      fst_->AddArc(fst_->Start(), A(0, olabel, Weight::One(), fst_->Start()));
      return;
    }
    StateId state = fst_->Start();
    for (std::size_t i = 0; i < ilabels.size() - 1; ++i) {
      const Label ilabel = ilabels[i];
      DCHECK_GT(ilabel, 0);
      auto key = std::make_pair(state, ilabel);
      auto iter = transition_.find(key);
      StateId nextstate;
      if (iter != transition_.end()) {
        nextstate = iter->second;
      } else {
        nextstate = fst_->AddState();
        fst_->AddArc(state, A(ilabel, 0, Weight::One(), nextstate));
        transition_.emplace(std::move(key), nextstate);
      }
      state = nextstate;
    }
    const Label ilabel = ilabels.back();
    DCHECK_GT(ilabel, 0);
    fst_->AddArc(state, A(ilabel, olabel, Weight::One(), fst_->Start()));
  }

 private:
  VectorFst<A> *fst_;  // not owned
  std::map<std::pair<StateId, Label>, StateId> transition_;
};

template <class A>
class SuffixTree {
 public:
  typedef typename A::Label Label;
  typedef typename A::StateId StateId;
  typedef typename A::Weight Weight;

  explicit SuffixTree(VectorFst<A> *fst) : fst_(fst) {
    auto start = AddStartFinal(fst_);
    suffix_.emplace(std::vector<Label>(), start);
  }

  void Add(Label ilabel, const std::vector<Label> &olabels) {
    DCHECK_GT(ilabel, 0);
    StateId state = fst_->Start();
    if (olabels.empty()) {
      fst_->AddArc(state, A(ilabel, 0, Weight::One(), state));
      return;
    }
    DCHECK_GT(olabels.front(), 0);
    bool first = true;
    for (auto iter = olabels.begin(); iter != olabels.end(); ++iter) {
      std::vector<Label> key(iter + 1, olabels.end());
      auto sfx_iter = suffix_.find(key);
      if (sfx_iter != suffix_.end()) {
        fst_->AddArc(state, A(ilabel, *iter, Weight::One(), sfx_iter->second));
        return;
      }
      StateId nextstate = fst_->AddState();
      fst_->AddArc(state, A(ilabel, *iter, Weight::One(), nextstate));
      suffix_.emplace(std::move(key), nextstate);
      state = nextstate;
      if (first) {
        first = false;
        ilabel = 0;
      }
    }
    LOG(DFATAL) << "Unreachable";
  }

  VectorFst<A> *GetFst() { return &fst_; }

 private:
  VectorFst<A> *fst_;  // not owned
  std::map<std::vector<Label>, StateId> suffix_;
};

void EscapeSymbol(const string &in, string *out) {
  for (auto c : in) {
    switch (c) {
      case ' ':  *out += "__"; break;
      case '\t': *out += "_t"; break;
      case '_':  *out += "_u"; break;
      case ';':  *out += "_s"; break;
      default:   *out += c; break;
    }
  }
}

bool UnescapeSymbol(const string &in, string *out) {
  for (string::size_type i = 0; i < in.size(); ++i) {
    auto c = in[i];
    if (c != '_') {
      *out += c;
      continue;
    }
    if (i + 1 >= in.size()) {
      LOG(ERROR) << "Incomplete escape at end of string: " << in;
      return false;
    }
    c = in[i + 1];
    switch (c) {
      case '_': *out += ' '; break;
      case 't': *out += '\t'; break;
      case 'u': *out += '_'; break;
      case 's': *out += ';'; break;
      default:
        LOG(ERROR) << "Unknown escape character '" << c << "' in string: "
                   << in;
        return false;
    }
  }
  return true;
}

}  // namespace

string AlignablesUtil::MakePairSymbol(const Alignable &alignable) {
  string result;
  EscapeSymbol(alignable.input(), &result);
  result += ";";
  EscapeSymbol(alignable.output(), &result);
  return result;
}

std::unique_ptr<AlignablesUtil> AlignablesUtil::FromFile(const string &path) {
  std::unique_ptr<AlignablesUtil> util;
  if (path.empty()) {
    LOG(ERROR) << "Path to alignables is empty";
    return util;
  }
  AlignablesSpec spec;
  if (!GetTextProtoFromFile(path.c_str(), &spec)) {
    return util;
  }
  VLOG(1) << "BEGIN AlignablesSpec\n"
          << spec.Utf8DebugString()
          << "END AlignablesSpec";
  util = FromSpec(spec);
  if (!util) {
    LOG(ERROR) << "Could not create AlignablesUtil from spec:\n"
               << spec.Utf8DebugString();
  }
  return util;
}

std::unique_ptr<AlignablesUtil> AlignablesUtil::FromSpec(
    const AlignablesSpec &spec) {
  std::unique_ptr<AlignablesUtil> util(new AlignablesUtil());
  if (!util->Init(spec)) {
    util.reset();
  }
  return util;
}

template <class A>
static void DeterminizeUnweighted(fst::MutableFst<A> *fst) {
  CHECK(fst->Properties(fst::kUnweighted, true));
  fst::VectorFst<A> tmp;
  fst::Determinize(*fst, &tmp);
  fst::ArcMap(tmp, fst, fst::RmWeightMapper<A>());
  CHECK(fst->Properties(fst::kUnweighted, true));
}

bool AlignablesUtil::Init(const AlignablesSpec &spec) {
  typedef Arc::Label Label;
  typedef Arc::Weight Weight;

  std::unique_ptr<SymbolTable> input_symbols;
  switch (spec.input_label_type()) {
    case ::festus::BYTE:
      input_label_maker_.reset(new ByteLabelMaker());
      break;
    case ::festus::SYMBOL:
      input_symbols.reset(new SymbolTable("input"));
      input_symbols->AddSymbol(kEpsilonSymbol, 0);
      for (const auto &symbol : spec.input_symbol()) {
        input_symbols->AddSymbol(symbol.first, symbol.second);
      }
      input_label_maker_.reset(new SymbolLabelMaker(input_symbols.get(), " "));
      break;
    case ::festus::UNICODE:
      input_label_maker_.reset(new UnicodeLabelMaker());
      break;
    default:
      LOG(ERROR) << "Unknown input label type: " << spec.input_label_type();
      return false;
  }

  std::unique_ptr<SymbolTable> output_symbols;
  switch (spec.output_label_type()) {
    case ::festus::BYTE:
      output_label_maker_.reset(new ByteLabelMaker());
      break;
    case ::festus::SYMBOL: {
      output_symbols.reset(new SymbolTable("output"));
      output_symbols->AddSymbol(kEpsilonSymbol, 0);
      for (const auto &symbol : spec.output_symbol()) {
        output_symbols->AddSymbol(symbol.first, symbol.second);
      }
      output_label_maker_.reset(
          new SymbolLabelMaker(output_symbols.get(), " "));
      break;
    }
    case ::festus::UNICODE:
      output_label_maker_.reset(new UnicodeLabelMaker());
      break;
    default:
      LOG(ERROR) << "Unknown output label type: " << spec.output_label_type();
      return false;
  }

  // Populate pair_fsa_ and its symbol table:
  SymbolTable pair_symbols("pair");
  pair_symbols.AddSymbol(kEpsilonSymbol, 0);
  auto state = AddStartFinal(&pair_fsa_);
  for (const Alignable &ali : spec.alignable()) {
    string pair_symbol = MakePairSymbol(ali);
    if (pair_symbols.Find(pair_symbol) != SymbolTable::kNoSymbol) {
      LOG(ERROR) << "Pair symbol " << pair_symbol << " for alignable { "
                 << ali.Utf8DebugString() << " } has already been defined";
      return false;
    }
    auto pair_label = pair_symbols.AddSymbol(pair_symbol);
    pair_fsa_.AddArc(state, Arc(pair_label, pair_label, Weight::One(), state));
  }
  pair_fsa_.SetInputSymbols(&pair_symbols);
  pair_fsa_.SetOutputSymbols(&pair_symbols);
  fst::ArcSort(&pair_fsa_, fst::ILabelCompare<Arc>());
  // TODO: Remove the previous line? Already arc-sorted by construction.

  // Populate (inverse) canonical projection FSTs and their symbol tables:
  PrefixTree<Arc> i2p(&input_to_pair_fst_), o2p(&output_to_pair_fst_);
  SuffixTree<Arc> p2i(&pair_to_input_fst_), p2o(&pair_to_output_fst_);
  std::vector<Label> labels;
  for (const Alignable &ali : spec.alignable()) {
    // TODO: Merge into previous loop.
    Label pair_label = pair_symbols.Find(MakePairSymbol(ali));
    CHECK_NE(pair_label, SymbolTable::kNoSymbol);

    const string &input = ali.input();
    const string &output = ali.output();

    input_label_maker_->StringToLabels(input, &labels);
    i2p.Add(labels, pair_label);
    p2i.Add(pair_label, labels);

    output_label_maker_->StringToLabels(output, &labels);
    o2p.Add(labels, pair_label);
    p2o.Add(pair_label, labels);
  }
  input_to_pair_fst_.SetInputSymbols(input_symbols.get());
  input_to_pair_fst_.SetOutputSymbols(&pair_symbols);
  fst::ArcSort(&input_to_pair_fst_, fst::ILabelCompare<Arc>());
  input_to_pair_fst_.Properties(fst::kFstProperties, true);

  output_to_pair_fst_.SetInputSymbols(output_symbols.get());
  output_to_pair_fst_.SetOutputSymbols(&pair_symbols);
  fst::ArcSort(&output_to_pair_fst_, fst::ILabelCompare<Arc>());
  output_to_pair_fst_.Properties(fst::kFstProperties, true);

  pair_to_input_fst_.SetInputSymbols(&pair_symbols);
  pair_to_input_fst_.SetOutputSymbols(input_symbols.get());
  fst::ArcSort(&pair_to_input_fst_, fst::ILabelCompare<Arc>());
  pair_to_input_fst_.Properties(fst::kFstProperties, true);

  pair_to_output_fst_.SetInputSymbols(&pair_symbols);
  pair_to_output_fst_.SetOutputSymbols(output_symbols.get());
  fst::ArcSort(&pair_to_output_fst_, fst::ILabelCompare<Arc>());
  pair_to_output_fst_.Properties(fst::kFstProperties, true);

  if (spec.forbidden_size() > 0) {
    const auto start = forbidden_factors_fsa_.AddState();
    forbidden_factors_fsa_.SetStart(start);
    const auto final = forbidden_factors_fsa_.AddState();
    forbidden_factors_fsa_.SetFinal(final, Weight::One());
    for (const ForbiddenFactor &fofa : spec.forbidden()) {
      auto state = start;
      if (fofa.alignable_size() < 2) {
        LOG(WARNING) << "Forbidden factor has length < 2: "
                     << fofa.Utf8DebugString();
      }
      for (int i = 0; i < fofa.alignable_size(); ++i) {
        const Alignable &ali = fofa.alignable(i);
        string pair_symbol = MakePairSymbol(ali);
        auto pair_label = pair_symbols.Find(pair_symbol);
        if (pair_label < 0) {
          LOG(ERROR) << "Pair symbol " << pair_symbol << " for alignable { "
                     << ali.Utf8DebugString() << " } not found";
          return false;
        }
        auto nextstate = final;
        if (i < fofa.alignable_size() - 1) {
          nextstate = forbidden_factors_fsa_.AddState();
        }
        forbidden_factors_fsa_.AddArc(
            state, Arc(pair_label, pair_label, Weight::One(), nextstate));
        state = nextstate;
      }
    }
    forbidden_factors_fsa_.SetInputSymbols(&pair_symbols);
    forbidden_factors_fsa_.SetOutputSymbols(&pair_symbols);
    DeterminizeUnweighted(&forbidden_factors_fsa_);
    fst::Concat(pair_fsa_, &forbidden_factors_fsa_);
    fst::Concat(&forbidden_factors_fsa_, pair_fsa_);
    fst::RmEpsilon(&forbidden_factors_fsa_);
    // This results in a huge blow-up of the FSA:
    DeterminizeUnweighted(&forbidden_factors_fsa_);
    fst::AcceptorMinimize(&forbidden_factors_fsa_);
    fst::ArcSort(&forbidden_factors_fsa_, fst::ILabelCompare<Arc>());
    uint64 props = forbidden_factors_fsa_.Properties(fst::kFstProperties, true);
    uint64 unoeps = fst::kUnweighted | fst::kNoEpsilons | fst::kIDeterministic |
        fst::kAcceptor;
    if ((props & unoeps) != unoeps) {
      LOG(WARNING) << "Forbidden factors FSA is not an unweighted epsilon-free"
          " deterministic acceptor";
    }
  }

  return true;
}

void AlignablesUtil::RemoveForbiddenFactors(
    fst::VectorFst<Arc> *pair_fsa) const {
  if (forbidden_factors_fsa_.Start() == fst::kNoStateId) {
    // Nothing to remove.
    return;
  }
  fst::VectorFst<Arc> tmp;
  fst::Difference(*pair_fsa, forbidden_factors_fsa_, &tmp);
  *pair_fsa = tmp;
}

}  // namespace festus
