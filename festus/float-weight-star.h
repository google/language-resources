// festus/float-weight-star.h
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
// Copyright 2015 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Unary Star operation for float weights.

#ifndef FESTUS_FLOAT_WEIGHT_STAR_H__
#define FESTUS_FLOAT_WEIGHT_STAR_H__

#include <cmath>

#include <fst/float-weight.h>

#include "festus/math-util.h"

namespace fst {

template <class T>
inline TropicalWeightTpl<T> Star(const TropicalWeightTpl<T> &w) {
  TropicalWeightTpl<T> res = TropicalWeightTpl<T>::NoWeight();
  if (w.Value() < 0) {
    res = FloatLimits<T>::NegInfinity();
  } else if (w.Value() >= 0) {
    res = 0;
  }  // Otherwise w.Value() is NaN, i.e. !w.Member(), so return NoWeight().
  return res;
}

inline TropicalWeightTpl<float> Star(const TropicalWeightTpl<float> &w) {
  return Star<float>(w);
}

inline TropicalWeightTpl<double> Star(const TropicalWeightTpl<double> &w) {
  return Star<double>(w);
}

// We extend the log semiring to be a complete star semiring, where Star(w) is
// defined as the infinite sum of the power series 1 + w + w^2 + w^3 + ...
// (under log semiring operations).
//
// It is convenient to recall that the log semiring is isomorphic to the
// nonnegative real semiring (cf. Fletcher 1980, CACM 23(6):350) over [0;inf].
// The Star of a nonnegative real weight r = exp(-w) is the limit of the
// geometric power series 1 + r + r^2 + ... (under standard addition and
// multiplication). This series does not converge for r >= 1, so we let the
// result be infinity (which becomes negative infinity in the LogWeight
// semiring). When the series converges, for |r| < 1, the limit is equal to
// 1/(1-r).
//
// Back in the log semiring, we need to return -log(1/(1-r)) == log(1-r) ==
// log(1-exp(-w)) == Log1mExp(-w). Taking logs implicitly takes care of the
// convergence check: log(1 - r) only takes on finite floating point values for
// r < 1.
template <class T>
inline LogWeightTpl<T> Star(const LogWeightTpl<T> &w) {
  return ::festus::Log1mExp(-w.Value());
}

inline LogWeightTpl<float> Star(const LogWeightTpl<float> &w) {
  return Star<float>(w);
}

inline LogWeightTpl<double> Star(const LogWeightTpl<double> &w) {
  return Star<double>(w);
}

template <class T>
inline MinMaxWeightTpl<T> Star(const MinMaxWeightTpl<T> &w) {
  MinMaxWeightTpl<T> res = MinMaxWeightTpl<T>::NoWeight();
  if (w.Member()) {
    res = FloatLimits<T>::NegInfinity();
  }
  return res;
}

inline MinMaxWeightTpl<float> Star(const MinMaxWeightTpl<float> &w) {
  return Star<float>(w);
}

inline MinMaxWeightTpl<double> Star(const MinMaxWeightTpl<double> &w) {
  return Star<double>(w);
}

}  // namespace fst

#endif  // FESTUS_FLOAT_WEIGHT_STAR_H__
