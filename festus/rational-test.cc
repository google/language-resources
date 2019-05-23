// festus/rational-test.cc
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
// Unit test for rational numbers.

#include "festus/rational.h"

#include <cstdint>
#include <cstring>
#include <limits>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

namespace {

using festus::Rational;

template <typename T>
void Is128bitPOD() {
  EXPECT_TRUE(sizeof(T) == 128 / 8);
  EXPECT_TRUE(std::is_standard_layout<T>::value);
  EXPECT_TRUE(std::is_trivial<T>::value);

  EXPECT_TRUE(std::is_trivially_default_constructible<T>::value);
  EXPECT_TRUE(std::is_trivially_copy_constructible<T>::value);
  EXPECT_TRUE(std::is_trivially_move_constructible<T>::value);
  EXPECT_TRUE(std::is_trivially_copy_assignable<T>::value);
  EXPECT_TRUE(std::is_trivially_move_assignable<T>::value);
  EXPECT_TRUE(std::is_trivially_destructible<T>::value);

  EXPECT_TRUE(std::is_nothrow_default_constructible<T>::value);
  EXPECT_TRUE(std::is_nothrow_copy_constructible<T>::value);
  EXPECT_TRUE(std::is_nothrow_move_constructible<T>::value);
  EXPECT_TRUE(std::is_nothrow_copy_assignable<T>::value);
  EXPECT_TRUE(std::is_nothrow_move_assignable<T>::value);
  EXPECT_TRUE(std::is_nothrow_destructible<T>::value);
}

TEST(RationalTest, TypeTraits) {
  Is128bitPOD<festus::rational_details::RationalBase>();
  Is128bitPOD<Rational>();
}

TEST(RationalTest, ConstantExpressions) {
  constexpr Rational zero = 0;
  constexpr Rational negative_zero = -zero;
  constexpr Rational one = 1;
  constexpr Rational negative_one = -one;
  constexpr Rational infinity = Rational::infinity();
  constexpr Rational negative_infinity = -infinity;
  constexpr Rational nan = Rational::quiet_NaN();
  constexpr Rational negative_nan = -nan;

  constexpr int zero_c = fpclassify(zero);
  EXPECT_EQ(FP_ZERO, zero_c);
  EXPECT_TRUE(isfinite(zero));
  EXPECT_FALSE(static_cast<bool>(zero));

  constexpr int negative_zero_c = fpclassify(negative_zero);
  EXPECT_EQ(FP_ZERO, negative_zero_c);
  EXPECT_TRUE(isfinite(negative_zero));
  EXPECT_FALSE(static_cast<bool>(negative_zero));

  constexpr int one_c = fpclassify(one);
  EXPECT_EQ(FP_NORMAL, one_c);
  EXPECT_TRUE(isfinite(one));
  EXPECT_TRUE(static_cast<bool>(one));

  constexpr int negative_one_c = fpclassify(negative_one);
  EXPECT_EQ(FP_NORMAL, negative_one_c);
  EXPECT_TRUE(isfinite(negative_one));
  EXPECT_TRUE(static_cast<bool>(negative_one));

  constexpr int infinity_c = fpclassify(infinity);
  EXPECT_EQ(FP_INFINITE, infinity_c);
  EXPECT_TRUE(isinf(infinity));
  EXPECT_TRUE(static_cast<bool>(infinity));

  constexpr int negative_infinity_c = fpclassify(negative_infinity);
  EXPECT_EQ(FP_INFINITE, negative_infinity_c);
  EXPECT_TRUE(isinf(negative_infinity));
  EXPECT_TRUE(static_cast<bool>(negative_infinity));

  constexpr int nan_c = fpclassify(nan);
  EXPECT_EQ(FP_NAN, nan_c);
  EXPECT_TRUE(isnan(nan));
  EXPECT_TRUE(static_cast<bool>(nan));

  constexpr int negative_nan_c = fpclassify(negative_nan);
  EXPECT_EQ(FP_NAN, negative_nan_c);
  EXPECT_TRUE(isnan(negative_nan));
  EXPECT_TRUE(static_cast<bool>(negative_nan));

  // Signed zero.
  constexpr bool zero_s = signbit(zero);
  EXPECT_FALSE(zero_s);

  constexpr bool negative_zero_s = signbit(negative_zero);
  EXPECT_TRUE(negative_zero_s);

  EXPECT_EQ(zero, zero);
  EXPECT_EQ(zero, negative_zero);
  EXPECT_EQ(negative_zero, zero);
  EXPECT_EQ(negative_zero, negative_zero);
}

TEST(RationalTest, LongDouble) {
  const long double e = -std::exp(1.0L);
  const Rational r = e;
  EXPECT_EQ(e, static_cast<long double>(r));
}

TEST(RationalTest, Epsilon) {
  std::cerr << std::numeric_limits<long double>::epsilon() << std::endl;
  std::cerr << festus::Decimal(Rational::epsilon()) << std::endl;
}

TEST(RationalTest, GCD) {
  using festus::rational_details::ConstexprGCD;
  EXPECT_EQ(0, ConstexprGCD(0, 0));
  EXPECT_EQ(13, ConstexprGCD(13, 0));
  EXPECT_EQ(13, ConstexprGCD(0, 13));
  EXPECT_EQ(13, ConstexprGCD(13, 13));

  EXPECT_EQ(1, ConstexprGCD(7, 13));
  constexpr auto gcd_12_105 = ConstexprGCD(12, 105);
  EXPECT_EQ(3, gcd_12_105);
}

TEST(RationalTest, Multiplication) {
  constexpr Rational q = Rational(4, 16);
  constexpr Rational r = Rational(8, 1024);
  constexpr Rational expected = Rational(1, 512);
  EXPECT_EQ(expected, q * r);
}

template <typename T>
std::string ToString(T t) {
  std::ostringstream strm;
  strm << t;
  return strm.str();
}

void TestRationalToString(const std::string &expected, Rational r) {
  std::string observed = ToString(r);
  EXPECT_EQ(expected, observed);
}

TEST(RationalTest, ToString) {
  TestRationalToString("12345678901", 12345678901ULL);
  TestRationalToString("0", 0);
  TestRationalToString("-0", -0.0);
  TestRationalToString("3*2**-2", 0.75F);
  TestRationalToString("3*2**-2", 0.75);
  TestRationalToString("3*2**-2", 0.75L);
  TestRationalToString("-1*2**63", std::numeric_limits<long long>::min());
  TestRationalToString(ToString(std::numeric_limits<long long>::max()),
                       std::numeric_limits<long long>::max());

  TestRationalToString("1*2**-126", std::numeric_limits<float>::min());
  TestRationalToString("1*2**-149", std::numeric_limits<float>::denorm_min());

  TestRationalToString("1*2**-1074", std::numeric_limits<double>::denorm_min());
  TestRationalToString("1*2**-1022", std::numeric_limits<double>::min());
  TestRationalToString("9007199254740991*2**971",  // (1 << 53), 1024 - 53
                       std::numeric_limits<double>::max());
  TestRationalToString("inf", std::numeric_limits<double>::infinity());
  TestRationalToString("-inf", -std::numeric_limits<double>::infinity());
  TestRationalToString("nan", -std::numeric_limits<double>::quiet_NaN());
  TestRationalToString("nan", -std::numeric_limits<double>::signaling_NaN());

  TestRationalToString("1*2**-16382", std::numeric_limits<long double>::min());
  TestRationalToString(
      "1*2**-16445", std::numeric_limits<long double>::denorm_min());

  EXPECT_EQ("-2.5e-2", ToString(festus::Decimal(Rational(-0.025))));
  EXPECT_EQ("1.13648e-161614258", ToString(festus::Decimal(Rational::min())));
}

#if 0
TEST(RationalTest, GCD) {
  using festus::rational_details::ConstexprGCD;
  EXPECT_EQ(1, ConstexprGCD(0, 1));
  EXPECT_EQ(1, ConstexprGCD(10, 17));
  EXPECT_EQ(1, ConstexprGCD(7, 10));
}
#endif

#if 0
TEST(RationalTest, Star) {
  using festus::Rational;
  EXPECT_FALSE(Rational(1).Star().Member());
  EXPECT_FALSE(Rational(2, 2).Star().Member());

  EXPECT_EQ(Rational::One(), Rational::Zero().Star());

  EXPECT_EQ(Rational::Approximate(-0.0), Rational::PosInfinity().Star());
  EXPECT_EQ(Rational::Approximate(+0.0), Rational::NegInfinity().Star());

  EXPECT_EQ(Rational::Approximate(3.2), Rational::Approximate(0.6875).Star());
  EXPECT_EQ(Rational::Approximate(16, 5), Rational::Approximate(11, 16).Star());
  EXPECT_EQ((Rational::Exact<16, 5>()), (Rational::Exact<11, 16>()).Star());

  EXPECT_EQ(Rational(10, 3), Rational(7, 10).Star());
  EXPECT_EQ(Rational(10, 17), Rational(-7, 10).Star());

  EXPECT_EQ(Rational(2048, 2049), Rational(-1, 2048).Star());
  EXPECT_EQ(Rational::Approximate(2048, 2049),
            Rational::Approximate(-1, 2048).Star());

  constexpr int64 kMaxInt = 1LL << 53;
  constexpr uint64 kMaxDenom = std::numeric_limits<uint64>::max();
  VLOG(0) << "kMaxInt: " << kMaxInt;
  VLOG(0) << "kMaxDenom: " << kMaxDenom;
  const Rational a = Rational::Approximate(-kMaxInt, kMaxDenom);
  VLOG(0) << a;
  EXPECT_EQ(-1.0 / 2048, a.DoubleValue());
  EXPECT_TRUE(festus::rational_details::IsExact(kMaxInt) && kMaxDenom);
  constexpr Rational x(-kMaxInt, kMaxDenom);
  VLOG(0) << x;
  EXPECT_EQ(-1.0 / 2048, x.DoubleValue());
  Rational y = x;
  y.Star();
  EXPECT_EQ(0.9995119570522206,y.DoubleValue());
  EXPECT_EQ(0.9995119570522206,
            (Rational::Exact<-kMaxInt, kMaxDenom>()).Star().DoubleValue());

  y = Rational(kMaxInt + 2, kMaxInt + 4);
  VLOG(0) << y;
  VLOG(0) << y.Star();
  EXPECT_TRUE(y.Member());

  Rational z(kMaxInt - 1, kMaxInt);
  VLOG(0) << z;
  VLOG(0) << z.Star();
  EXPECT_EQ(Rational(kMaxInt), z);
  EXPECT_FALSE(Rational(kMaxInt + 1, kMaxInt).Star().Member());

  // Test case: -3/((1<<63)-1)
  // Test case: -7/((1<<64)-1)

}
#endif

}  // namespace
