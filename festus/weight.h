// festus/weight.h
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
// Façades for OpenFst weights.

#ifndef FESTUS_WEIGHT_H__
#define FESTUS_WEIGHT_H__

#include <cstddef>
#include <functional>
#include <istream>
#include <ostream>
#include <type_traits>

#include <fst/compat.h>
#include <fst/weight.h>

namespace festus {

template <int> struct PrecisionString {};

template <> struct PrecisionString<1> {
  static string Get() { return "8"; }
};

template <> struct PrecisionString<2> {
  static string Get() { return "16"; }
};

template <> struct PrecisionString<4> {
  static string Get() { return "32"; }
};

template <> struct PrecisionString<8> {
  static string Get() { return "64"; }
};

template <> struct PrecisionString<16> {
  static string Get() { return "128"; }
};

// OpenFst weight façade for semirings whose elements are passed by value.
template <class S>
class ValueWeightTpl {
 public:
  typedef typename S::ValueType ValueType;
  typedef ValueWeightTpl ReverseWeight;

  ValueWeightTpl() = default;
  ~ValueWeightTpl() = default;
  ValueWeightTpl(ValueWeightTpl &&) = default;
  ValueWeightTpl(const ValueWeightTpl &) = default;
  ValueWeightTpl &operator=(ValueWeightTpl &&) = default;
  ValueWeightTpl &operator=(const ValueWeightTpl &) = default;

  constexpr ValueWeightTpl(ValueType value) : value_(value) {}

  ValueWeightTpl &operator=(ValueType value) {
    value_ = value;
    return *this;
  }

  ValueType Value() const { return value_; }

  static const ValueWeightTpl NoWeight() {
    return ValueWeightTpl(S::NoWeight());
  }

  static constexpr ValueWeightTpl Zero() { return ValueWeightTpl(S::Zero()); }

  static constexpr ValueWeightTpl One() { return ValueWeightTpl(S::One()); }

  friend inline ValueWeightTpl Plus(ValueWeightTpl lhs, ValueWeightTpl rhs) {
    lhs.value_ = S::Plus(lhs.value_, rhs.value_);
    return lhs;
  }

  friend inline ValueWeightTpl Minus(ValueWeightTpl lhs, ValueWeightTpl rhs) {
    lhs.value_ = S::Minus(lhs.value_, rhs.value_);
    return lhs;
  }

  friend inline ValueWeightTpl Times(ValueWeightTpl lhs, ValueWeightTpl rhs) {
    lhs.value_ = S::Times(lhs.value_, rhs.value_);
    return lhs;
  }

  friend inline ValueWeightTpl Divide(ValueWeightTpl lhs, ValueWeightTpl rhs,
                                      fst::DivideType typ = fst::DIVIDE_ANY) {
    lhs.value_ = S::Divide(lhs.value_, rhs.value_);
    return lhs;
  }

  friend inline ValueWeightTpl Star(ValueWeightTpl w) {
    w.value_ = S::Star(w.value_);
    return w;
  }

  ValueWeightTpl Reverse() const { return ValueWeightTpl(S::Reverse(value_)); }

  ValueWeightTpl Quantize(float delta = fst::kDelta) const {
    return ValueWeightTpl(S::Quantize(value_, delta));
  }

  constexpr bool Member() const { return S::Member(value_); }

  friend inline bool operator==(ValueWeightTpl lhs, ValueWeightTpl rhs) {
    return S::EqualTo(lhs.value_, rhs.value_);
  }

  friend inline bool operator!=(ValueWeightTpl lhs, ValueWeightTpl rhs) {
    return !S::EqualTo(lhs.value_, rhs.value_);
  }

  friend inline bool ApproxEqual(ValueWeightTpl lhs, ValueWeightTpl rhs,
                                 float delta = fst::kDelta) {
    return S::ApproxEqual(lhs.value_, rhs.value_, delta);
  }

  std::size_t Hash() const { return std::hash<ValueType>()(value_); }

  std::istream &Read(std::istream &strm) {
    static_assert(std::is_pod<ValueType>::value, "value type should be POD");
    return strm.read(reinterpret_cast<char *>(&value_), sizeof(value_));
  }

  std::ostream &Write(std::ostream &strm) const {
    return strm.write(reinterpret_cast<const char *>(&value_), sizeof(value_));
  }

  static const string &Type() {
    static const string type = S::Name() +
        (sizeof(value_) == 4 ? "" : PrecisionString<sizeof(value_)>::Get());
    return type;
  }

  static constexpr uint64 Properties() { return S::kProperties; }

 private:
  ValueType value_;
};

}  // namespace festus

#endif  // FESTUS_WEIGHT_H__
