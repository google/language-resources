// festus/arc.h
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
// FST arc types for use with C++11.

#ifndef FESTUS_ARC_H__
#define FESTUS_ARC_H__

#include <utility>

#include <fst/arc.h>
#include <fst/compat.h>

namespace festus {

// Arc template for use with weights passed by reference.
template <class W>
class ReferenceArcTpl {
 public:
  typedef W Weight;
  typedef fst::ArcTpl<char>::Label Label;
  typedef fst::ArcTpl<char>::StateId StateId;

  constexpr ReferenceArcTpl(Label i, Label o, W &&w, StateId s)
      : ilabel(i), olabel(o), weight(std::move(w)), nextstate(s) {}

  constexpr ReferenceArcTpl(Label i, Label o, const W &w, StateId s)
      : ilabel(i), olabel(o), weight(w), nextstate(s) {}

  ReferenceArcTpl() = default;
  ~ReferenceArcTpl() = default;
  ReferenceArcTpl(ReferenceArcTpl &&) = default;
  ReferenceArcTpl(const ReferenceArcTpl &) = default;
  ReferenceArcTpl &operator=(ReferenceArcTpl &&) = default;
  ReferenceArcTpl &operator=(const ReferenceArcTpl &) = default;

  static const string &Type() {
    static const string type =
        Weight::Type() == "tropical" ? "standard" : Weight::Type();
    return type;
  }

  Label ilabel;
  Label olabel;
  Weight weight;
  StateId nextstate;
};

// Arc template for use with weights passed by value.
template <class W>
class ValueArcTpl {
 public:
  typedef W Weight;
  typedef fst::ArcTpl<char>::Label Label;
  typedef fst::ArcTpl<char>::StateId StateId;

  constexpr ValueArcTpl(Label i, Label o, Weight w, StateId s)
      : ilabel(i), olabel(o), weight(w), nextstate(s) {}

  ValueArcTpl() = default;
  ~ValueArcTpl() = default;
  ValueArcTpl(ValueArcTpl &&) = default;
  ValueArcTpl(const ValueArcTpl &) = default;
  ValueArcTpl &operator=(ValueArcTpl &&) = default;
  ValueArcTpl &operator=(const ValueArcTpl &) = default;

  static const string &Type() {
    static const string type =
        Weight::Type() == "tropical" ? "standard" : Weight::Type();
    return type;
  }

  Label ilabel;
  Label olabel;
  Weight weight;
  StateId nextstate;
};

}  // namespace festus

#endif  // FESTUS_ARC_H__
