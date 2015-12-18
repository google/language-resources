// festus/math-util.h
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
// Numerical utility functions.

#ifndef FESTUS_MATH_UTIL_H__
#define FESTUS_MATH_UTIL_H__

#include <cmath>
#include <limits>

namespace festus {
namespace internal {

template <class T>
struct NumericConstants {
  // The largest value (upper bound) x for which std::exp<T>(x) == 0,
  // i.e. sup {x | exp(x) == 0}.
  static constexpr T SupExpEqZero() noexcept { return T(); }

  // The smallest value (lower bound) x for which std::exp<T>(x) >= 1,
  // i.e. inf {x | exp(x) >= 1}.
  static constexpr T InfExpGeOne() noexcept { return T(); }
};

template <>
struct NumericConstants<float> {
  static constexpr float SupExpEqZero() noexcept { return -103.972084F; }
  static constexpr float InfExpGeOne() noexcept { return -2.98023224e-8F; }
};

template <>
struct NumericConstants<double> {
  static constexpr double SupExpEqZero() noexcept {
    return -745.13321910194122;
  }
  static constexpr double InfExpGeOne() noexcept {
    return -5.5511151231257827e-17;
  }
};

}  // namespace internal

#if 0  // disabled after profiling

// Computes log(1 - exp(r)). If r >= 0, returns negative infinity.
template <class T>
inline T Log1mExp(T r) {
  if (std::isnan<T>(r)) {
    return r;
  } else if (r <= internal::NumericConstants<T>::SupExpEqZero()) {
    // Here exp(r) == 0, so always return log(1 - 0), which is 0.
    return static_cast<T>(0);
  } else if (r >= internal::NumericConstants<T>::InfExpGeOne()) {
    // Here exp(r) >= 1, so always return log(1 - 1), which is -inf.
    return -std::numeric_limits<T>::infinity();
  } else {
    return std::log1p<T>(-std::exp<T>(r));
  }
}

#else  // simpler implementation

// Computes log(1 - exp(r)). If r >= 0, returns negative infinity.
template <class T>
inline T Log1mExp(T r) {
  if (r <= internal::NumericConstants<T>::SupExpEqZero()) {
    // Here exp(r) == 0, so always return log(1 - 0), which is 0.
    return static_cast<T>(0);
  } else {
    return std::log1p(-std::exp(r));
  }
}

#endif

}  // namespace festus

#endif  // FESTUS_MATH_UTIL_H__
