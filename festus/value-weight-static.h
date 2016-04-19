// festus/value-weight-static.h
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
// Façade for OpenFst weights around a semiring class with static methods.

#ifndef FESTUS_VALUE_WEIGHT_STATIC_H__
#define FESTUS_VALUE_WEIGHT_STATIC_H__

#include <cstddef>
#include <functional>
#include <istream>
#include <ostream>
#include <type_traits>
#include <utility>

#include <fst/compat.h>
#include <fst/weight.h>

namespace festus {

// OpenFst weight façade for semirings whose elements are passed by value.
//
// This version works with a semiring class S with static member functions.
template <class S>
class ValueWeightStatic {
 public:
  typedef S SemiringType;
  typedef typename S::ValueType ValueType;
  typedef ValueWeightStatic ReverseWeight;

  ValueWeightStatic() = default;
  ~ValueWeightStatic() = default;
  ValueWeightStatic(ValueWeightStatic &&) = default;
  ValueWeightStatic(const ValueWeightStatic &) = default;
  ValueWeightStatic &operator=(ValueWeightStatic &&) = default;
  ValueWeightStatic &operator=(const ValueWeightStatic &) = default;

  // Deprecated implicit constructor. Use From() instead.
  constexpr ValueWeightStatic(ValueType value) : value_(value) {}

  template <typename... Args>
  static constexpr ValueWeightStatic From(Args &&... args) {
    return ValueWeightStatic(SemiringType::From(std::forward<Args>(args)...));
  }

  constexpr ValueType Value() const { return value_; }

  static constexpr ValueWeightStatic NoWeight() {
    return ValueWeightStatic(SemiringType::NoWeight());
  }

  static constexpr ValueWeightStatic Zero() {
    return ValueWeightStatic(SemiringType::Zero());
  }

  static constexpr ValueWeightStatic One() {
    return ValueWeightStatic(SemiringType::One());
  }

  friend inline ValueWeightStatic Plus(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs) {
    lhs.value_ = SemiringType::OpPlus(lhs.value_, rhs.value_);
    return lhs;
  }

  friend inline ValueWeightStatic Minus(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs) {
    lhs.value_ = SemiringType::OpMinus(lhs.value_, rhs.value_);
    return lhs;
  }

  friend inline ValueWeightStatic Times(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs) {
    lhs.value_ = SemiringType::OpTimes(lhs.value_, rhs.value_);
    return lhs;
  }

  friend inline ValueWeightStatic Divide(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs,
      fst::DivideType typ = fst::DIVIDE_ANY) {
    if (SemiringType::Commutative()) {
      return DivideCommutative(lhs, rhs);
    } else {
      return DivideNoncommutative(lhs, rhs, typ);
    }
  }

  friend inline ValueWeightStatic Star(
      ValueWeightStatic w) {
    w.value_ = SemiringType::OpStar(w.value_);
    return w;
  }

  constexpr ValueWeightStatic Reverse() const {
    return ValueWeightStatic(SemiringType::Reverse(value_));
  }

  constexpr ValueWeightStatic Quantize(float delta = fst::kDelta) const {
    return ValueWeightStatic(SemiringType::Quantize(value_, delta));
  }

  constexpr bool Member() const { return SemiringType::Member(value_); }

  friend constexpr bool operator==(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs) {
    return SemiringType::EqualTo(lhs.value_, rhs.value_);
  }

  friend constexpr bool operator!=(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs) {
    return !SemiringType::EqualTo(lhs.value_, rhs.value_);
  }

  friend constexpr bool ApproxEqual(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs,
      float delta = fst::kDelta) {
    return SemiringType::ApproxEqualTo(lhs.value_, rhs.value_, delta);
  }

  constexpr std::size_t Hash() const { return std::hash<ValueType>()(value_); }

  friend std::ostream &operator<<(
      std::ostream &strm,
      ValueWeightStatic w) {
    return SemiringType::Print(strm, w.value_);
  }

  std::istream &Read(std::istream &strm) {
    static_assert(std::is_pod<ValueType>::value, "value type should be POD");
    return strm.read(reinterpret_cast<char *>(&value_), sizeof(value_));
  }

  std::ostream &Write(std::ostream &strm) const {
    return strm.write(reinterpret_cast<const char *>(&value_), sizeof(value_));
  }

  static const string &Type() {
    static const string type = SemiringType::Name();
    return type;
  }

  static constexpr uint64 Properties() {
    return fst::kSemiring
        | (SemiringType::Commutative() ? fst::kCommutative : 0)
        | (SemiringType::Idempotent()  ? fst::kIdempotent  : 0);
  }

 private:
  static ValueWeightStatic DivideCommutative(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs) {
    lhs.value_ = SemiringType::OpDivide(lhs.value_, rhs.value_);
    return lhs;
  }

  static ValueWeightStatic DivideNoncommutative(
      ValueWeightStatic lhs,
      ValueWeightStatic rhs,
      fst::DivideType typ) {
    switch (typ) {
      case fst::DIVIDE_LEFT: {
        auto recip = SemiringType::Reciprocal(rhs.value_);
        lhs.value_ = SemiringType::OpTimes(recip, lhs.value_);
        break;
      }
      case fst::DIVIDE_RIGHT: {
        auto recip = SemiringType::Reciprocal(rhs.value_);
        lhs.value_ = SemiringType::OpTimes(lhs.value_, recip);
        break;
      }
      case fst::DIVIDE_ANY:
        FSTERROR() << "Only explicit left or right division is defined "
                   << "for the noncommutative " << Type() << " semiring";
        lhs.value_ = SemiringType::NoWeight();
        break;
    }
    return lhs;
  }

  ValueType value_;
};

}  // namespace festus

#endif  // FESTUS_VALUE_WEIGHT_STATIC_H__
