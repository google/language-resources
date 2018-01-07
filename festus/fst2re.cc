// festus/fst2re.cc
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
// Converts an FST to a crude regular expression.
//
// Only intended for toy cases, mostly as an illustration of TermSemiring.

#include <cstring>
#include <iostream>
#include <ostream>
#include <memory>

#include <fst/compat.h>
#include <fst/fstlib.h>

#include "festus/algebraic-path.h"
#include "festus/arc.h"
#include "festus/expression.pb.h"
#include "festus/term-semiring.h"
#include "festus/value-weight-singleton.h"

namespace festus {

template <class F>
inline ExpressionGraph LanguageOf(const F &fst);

std::ostream &PrintAsRegexAux(
    std::ostream &strm,
    const fst::SymbolTable *symbols,
    const ExpressionGraph &graph,
    uint32 node,
    Expression::ExpressionCase parent_case = Expression::kPlus);

inline std::ostream &PrintAsRegex(
    std::ostream &strm,
    const ExpressionGraph &graph,
    const fst::SymbolTable *symbols) {
  return PrintAsRegexAux(strm, symbols, graph, graph.root());
}

template <class A>
class ILabelTermMapper {
 public:
  typedef A FromArc;
  typedef typename A::Weight FromWeight;
  typedef typename A::StateId StateId;

  typedef BasicTermSemiring<FreeSemiringProperties> Semiring;
  typedef ValueWeightSingleton<Semiring, DefaultStaticInstance<Semiring>>
      ToWeight;
  typedef ValueArcTpl<ToWeight> ToArc;

  ILabelTermMapper() = delete;

  explicit ILabelTermMapper(const fst::Fst<A> &fst) : fst_(fst) {}

  explicit ILabelTermMapper(const ILabelTermMapper &mapper,
                            const fst::Fst<A> *fst = nullptr)
      : fst_(fst ? *fst : mapper.fst_) {
  }

  StateId Start() const { return fst_.Start(); }

  ToArc::Weight Final(StateId state) const {
    return fst_.Final(state) == FromWeight::Zero() ? ToWeight::Zero()
        : ToWeight::One();
  }

  void SetState(StateId state) {
    aiter_.reset(new fst::ArcIterator<fst::Fst<A>>(fst_, state));
  }

  bool Done() const { return aiter_->Done(); }

  ToArc Value() const {
    const FromArc &arc = aiter_->Value();
    ToArc to_arc(arc.ilabel, arc.olabel, ToWeight::One(), arc.nextstate);
    if (arc.weight == FromWeight::Zero()) {
      to_arc.weight = ToWeight::Zero();
    } else if (arc.ilabel != 0) {
      // Encode ilabel as leaf value in the free term semiring.
      to_arc.weight = ToWeight::From(arc.ilabel);
    }
    return to_arc;
  }

  void Next() { aiter_->Next(); }

  constexpr fst::MapSymbolsAction InputSymbolsAction() const {
    return fst::MAP_COPY_SYMBOLS;
  }

  constexpr fst::MapSymbolsAction OutputSymbolsAction() const {
    return fst::MAP_COPY_SYMBOLS;
  }

  constexpr uint64 Properties(uint64 props) const {
    return (props & fst::kWeightInvariantProperties) | fst::kWeighted;
  }

 private:
  const fst::Fst<A> &fst_;
  std::unique_ptr<fst::ArcIterator<fst::Fst<A>>> aiter_;
};

template <class Arc, class Mapper>
ExpressionGraph PathsOf(const fst::Fst<Arc> &fst) {
  Mapper mapper(fst);
#if 0
  // At OpenFst release 1.5.2, there is a limitation in fst::StateMapFst which
  // precludes the instantiation of fst::StateMapFst<A, B> where A and B are
  // distinct arc types. Work around this by copying the input to a VectorFst.
  fst::StateMapFst<Arc, typename Mapper::ToArc, Mapper> mapped_fst(
      fst, &mapper);
#endif
  fst::VectorFst<typename Mapper::ToArc> mapped_fst;
  fst::StateMap<Arc, typename Mapper::ToArc, Mapper>(
      fst, &mapped_fst, &mapper);
  // At this point mapped_fst has weights of type BasicTermWeight, which
  // contain handles created by a stateless TermSemiring instance. As these
  // handles do not depend on the memo of a TermSemiring, they have the same
  // meaning in all TermSemiring instantiations. This allows us to now switch
  // transparently to a stateful TermSemiring for computing the total path
  // weight. Since this stateful semiring object is local to this function, it
  // does not require synchronization.
  TermSemiring<UnsynchronizedMemo, FreeSemiringProperties> semiring;
  auto sum_total = SumTotalValue(mapped_fst, &semiring);
  return semiring.ToGraph(sum_total);
}

template <class Arc>
inline ExpressionGraph LanguageOf(const fst::Fst<Arc> &fst) {
  return PathsOf<Arc, ILabelTermMapper<Arc>>(fst);
}

std::ostream &PrintAsRegexAux(
    std::ostream &strm,
    const fst::SymbolTable *symbols,
    const ExpressionGraph &graph,
    uint32 node,
    Expression::ExpressionCase parent_case) {
  auto iter = graph.node().find(node);
  CHECK_NE(iter, graph.node().end());
  const auto &expression = iter->second;
  switch (expression.expression_case()) {
    case Expression::kZero:
      return strm << "{}";
    case Expression::kOne:
      return strm << "<epsilon>";
    case Expression::kError:
      return strm << "ERROR(" << expression.error() << ")";
    case Expression::kLeaf:
      if (symbols) {
        return strm << symbols->Find(expression.leaf().payload());
      } else {
        return strm << expression.leaf().payload();
      }
    case Expression::kPlus: {
      bool needs_parens = (parent_case != Expression::kPlus);
      if (needs_parens) strm << "(";
      PrintAsRegexAux(strm, symbols, graph, expression.plus().child1(),
                      expression.expression_case());
      strm << " | ";
      PrintAsRegexAux(strm, symbols, graph, expression.plus().child2(),
                      expression.expression_case());
      if (needs_parens) strm << ")";
      return strm;
    }
    case Expression::kTimes: {
      bool needs_parens = (parent_case != Expression::kPlus &&
                           parent_case != Expression::kTimes);
      if (needs_parens) strm << "(";
      PrintAsRegexAux(strm, symbols, graph, expression.times().child1(),
                      expression.expression_case());
      strm << " ";
      PrintAsRegexAux(strm, symbols, graph, expression.times().child2(),
                      expression.expression_case());
      if (needs_parens) strm << ")";
      return strm;
    }
    case Expression::kStar:
      PrintAsRegexAux(strm, symbols, graph, expression.star().child(),
                      expression.expression_case());
      return strm << "*";
    case Expression::kKleenePlus:
      PrintAsRegexAux(strm, symbols, graph, expression.kleene_plus().child(),
                      expression.expression_case());
      return strm << "+";
    default:
      return strm << "Error(NOT IMPLEMENTED)";
  }
}

}  // namespace festus

const char kUsage[] =
    R"(Converts a given FST to a crude regular expression.

If the FST is not an acceptor, it will be implicitly projected onto
its input tape (i.e. the output labels will be ignored).

Usage:
  fst2re [in.fst]
)";

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc > 2) {
    ShowUsage();
    return 2;
  }

  string in = (argc > 1 && std::strcmp(argv[1], "-") != 0) ? argv[1] : "";

  std::unique_ptr<fst::StdFst> fst(fst::StdFst::Read(in));
  if (!fst) return 2;

  auto graph = festus::LanguageOf(*fst);
  VLOG(1) << graph.DebugString();
  festus::PrintAsRegex(std::cout, graph, fst->InputSymbols()) << std::endl;

  return 0;
}
