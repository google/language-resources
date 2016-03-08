// festus/runtime/compact.h
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
// Compactors for use with OpenFst's CompactFst template class.

#ifndef FESTUS_RUNTIME_COMPACT_H__
#define FESTUS_RUNTIME_COMPACT_H__

#include <cstdio>
#include <istream>
#include <ostream>

#include <fst/compact-fst.h>
#include <fst/compat.h>
#include <fst/fst.h>

namespace festus {

template <class A, int ILABEL_BITS, int OLABEL_BITS, int NEXTSTATE_BITS>
class UnweightedBitfieldCompactor {
 public:
  typedef A Arc;
  typedef typename A::Label Label;
  typedef typename A::StateId StateId;
  typedef typename A::Weight Weight;

  struct Element {
    unsigned int ilabel : ILABEL_BITS;
    unsigned int olabel : OLABEL_BITS;
    unsigned int nextstate : NEXTSTATE_BITS;
  };

  static constexpr unsigned int kMaxILabel = (1 << ILABEL_BITS) - 1;
  static constexpr unsigned int kMaxOLabel = (1 << OLABEL_BITS) - 1;
  static constexpr unsigned int kMaxState = (1 << NEXTSTATE_BITS) - 1;

  Element Compact(StateId s, const A &arc) const {
    Element element;
    if (arc.ilabel == fst::kNoLabel) {
      element.ilabel = kMaxILabel;
    } else {
      CHECK_GE(arc.ilabel, 0);
      if (arc.ilabel < kMaxILabel) {
        element.ilabel = arc.ilabel;
      } else {
        LOG(ERROR) << "Input label too large: " << arc.ilabel;
        element.ilabel = kMaxILabel;
      }
    }
    if (arc.olabel == fst::kNoLabel) {
      element.olabel = kMaxOLabel;
    } else {
      CHECK_GE(arc.olabel, 0);
      if (arc.olabel < kMaxOLabel) {
        element.olabel = arc.olabel;
      } else {
        LOG(ERROR) << "Output label too large: " << arc.olabel;
        element.olabel = kMaxOLabel;
      }
    }
    if (arc.weight == Weight::Zero()) {
      LOG(ERROR) << "Arc weight is Zero; this cannot happen!";
    } else if (arc.weight != Weight::One()) {
      LOG(ERROR) << "Setting non-Zero arc weight to One: " << arc.weight;
    }
    if (arc.nextstate == fst::kNoStateId) {
      element.nextstate = kMaxState;
    } else {
      CHECK_GE(arc.nextstate, 0);
      if (arc.nextstate < kMaxState) {
        element.nextstate = arc.nextstate;
      } else {
        LOG(ERROR) << "Target state ID too large: " << arc.nextstate;
        element.nextstate = kMaxState;
      }
    }
    return element;
  }

  Arc Expand(StateId s, const Element &e,
             uint32 f = fst::kArcValueFlags) const {
    Label ilabel = e.ilabel == kMaxILabel ? fst::kNoLabel : e.ilabel;
    Label olabel = e.olabel == kMaxOLabel ? fst::kNoLabel : e.olabel;
    StateId next = e.nextstate == kMaxState ? fst::kNoStateId : e.nextstate;
    return Arc(ilabel, olabel, Weight::One(), next);
  }

  ssize_t Size() const { return -1; }

  uint64 Properties() const { return fst::kUnweighted; }

  bool Compatible(const fst::Fst<A> &fst) const {
    uint64 props = Properties();
    return fst.Properties(props, true) == props;
  }

  static const string &Type() {
    static const string type = MakeTypeName();
    return type;
  }

  bool Write(std::ostream &strm) const { return true; }

  static UnweightedBitfieldCompactor *Read(std::istream &strm) {
    return new UnweightedBitfieldCompactor();
  }

 private:
  static string MakeTypeName() {
    char buf[48];
    int len = std::snprintf(buf, sizeof(buf),
                            "bitfield_%d_%d_0_%d",
                            ILABEL_BITS, OLABEL_BITS, NEXTSTATE_BITS);
    return string(buf, len);
  }
};

template <class A>
using Compactor_8_10_0_14 = UnweightedBitfieldCompactor<A, 8, 10, 14>;

static_assert(sizeof(Compactor_8_10_0_14<fst::StdArc>::Element[8])
              ==               8+10+0+14,
              "Unexpected size of Element");

template <class A>
using Compact_8_10_0_14_Fst = fst::CompactFst<A, Compactor_8_10_0_14<A>>;

typedef Compact_8_10_0_14_Fst<fst::StdArc> StdCompact_8_10_0_14_Fst;
typedef Compact_8_10_0_14_Fst<fst::LogArc> LogCompact_8_10_0_14_Fst;
typedef Compact_8_10_0_14_Fst<fst::Log64Arc> Log64Compact_8_10_0_14_Fst;

}  // namespace festus

#endif  // FESTUS_RUNTIME_COMPACT_H__
