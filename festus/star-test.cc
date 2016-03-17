// festus/star-test.cc
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
//
// \file
// Unit test for Star operation.

#include "festus/float-weight-star.h"
#include "festus/real-weight.h"

#include <cmath>
#include <limits>

#include <fst/float-weight.h>
#include <gtest/gtest.h>

namespace {

constexpr double inf = std::numeric_limits<double>::infinity();

const float kTestValues[] = {
  0,
  std::numeric_limits<float>::denorm_min(),
  std::numeric_limits<float>::min(),
  0.0625, 0.25, 0.5, 1, 2, 4, 8, 16, 64, 256, 4096, 65536,
  std::numeric_limits<float>::max(),
  std::numeric_limits<float>::infinity(),
};

template <class Weight>
void TestUniversalProperties(float threshold_near = 1e-12) {
  // Star of a non-member is a non-member.
  EXPECT_FALSE(Star(Weight::NoWeight()).Member());

  // Star of Zero is always One.
  // This is because 0* == 1 + 0 0* == 1 + 0 == 1.
  EXPECT_EQ(Weight::One(), Star(Weight::Zero()));

  // In a semiring with idempotent Plus, w* == 1 + w*. This holds because
  // 1 + w* == 1 + (1 + w w*) == (1 + 1) + w w* == 1 + w w* == w*, where the
  // identity 1 + 1 == 1 follows from (and requires) idempotence.
  //
  // Furthermore in a complete semiring with idempotent Plus, w* == (1 + w)*.
  // This follows from the auxiliary fact that (1+w)^n == \sum_{i=0}^n w^i
  // (proof by induction) and therefore
  //
  //   (1 + w)* == \sum_{n=0}^{\infty} \sum_{i=0}^n w^i
  //            == \sum_{n=0}^{\infty} w^n
  //            == w*,
  //
  // since all repeated terms in the sums collapse due to idempotence. Here we
  // test these identities for various floating-point values (assumes Weight can
  // be constructed from a float).
  //
  // By the same logic 1* is an infinte sum of all natural powers of 1, namely
  // 1* == 1^0 + 1^1 + 1^2 + 1^3 + ... == 1 + 1 + 1 + 1 + ... which all collapse
  // into 1 by idempotence, i.e. 1* == 1.
  if (Weight::Properties() & fst::kIdempotent) {
    EXPECT_EQ(Weight::One(), Star(Weight::One()));
    for (float x : kTestValues) {
      for (float s : {+1.0F, -1.0F}) {
        Weight w = std::copysign(x, s);
        Weight star = Star(w);
        Weight one_plus_star = Plus(Weight::One(), star);
        Weight star1 = Star(Plus(Weight::One(), w));
        if (!star.Member()) {
          EXPECT_FALSE(one_plus_star.Member());
          EXPECT_FALSE(star1.Member());
        } else {
          EXPECT_FLOAT_EQ(star.Value(), one_plus_star.Value());
          EXPECT_FLOAT_EQ(star.Value(), star1.Value());
        }
      }
    }
  }

  // Test the Star axiom [w* == 1 + w w* == 1 + w* w] for a various
  // floating-point values (assumes Weight can be constructed from a float).
  for (float x : kTestValues) {
    for (float s : {+1.0F, -1.0F}) {
      Weight w = std::copysign(x, s);
      Weight star = Star(w);
      if (!w.Member() || !star.Member()) {
        continue;
      }
      Weight one_plus_w_star = Plus(Weight::One(), Times(w, star));
      Weight one_plus_star_w = Plus(Weight::One(), Times(star, w));
      EXPECT_TRUE(one_plus_w_star.Member());
      EXPECT_TRUE(one_plus_star_w.Member());
      if (std::isinf(star.Value())) {
        EXPECT_EQ(star, one_plus_w_star);
        EXPECT_EQ(star, one_plus_star_w);
      } else if (star.Value() < 1e-4) {
        EXPECT_NEAR(star.Value(), one_plus_w_star.Value(), threshold_near);
        EXPECT_NEAR(star.Value(), one_plus_star_w.Value(), threshold_near);
      } else {
        EXPECT_FLOAT_EQ(star.Value(), one_plus_w_star.Value());
        EXPECT_FLOAT_EQ(star.Value(), one_plus_star_w.Value());
      }
    }
  }
}

TEST(StarTest, UniversalPropertiesTropical) {
  TestUniversalProperties<fst::TropicalWeightTpl<float>>();
  TestUniversalProperties<fst::TropicalWeightTpl<double>>();
}

TEST(StarTest, UniversalPropertiesLog) {
  TestUniversalProperties<fst::LogWeightTpl<float>>(1e-7);
  TestUniversalProperties<fst::LogWeightTpl<double>>();
}

TEST(StarTest, UniversalPropertiesMinMax) {
  TestUniversalProperties<fst::MinMaxWeightTpl<float>>();
  TestUniversalProperties<fst::MinMaxWeightTpl<double>>();
}

TEST(StarTest, UniversalPropertiesReal) {
  TestUniversalProperties<festus::RealWeightTpl<float>>(1e-7);
  TestUniversalProperties<festus::RealWeightTpl<double>>();
}

template <class T>
void TestMinMax() {
  typedef fst::MinMaxWeightTpl<T> Weight;
  for (float x : kTestValues) {
    for (float s : {+1.0F, -1.0F}) {
      Weight w = std::copysign(x, s);
      EXPECT_EQ(Weight::One(), Star(w));
    }
  }
}

TEST(StarTest, MinMax) {
  TestMinMax<float>();
  TestMinMax<double>();
}

template <class T>
void TestTropical() {
  typedef fst::TropicalWeightTpl<T> Weight;
  for (float x : kTestValues) {
    for (float s : {+1.0F, -1.0F}) {
      Weight w = std::copysign(x, s);
      if (w.Value() >= 0) {
        EXPECT_EQ(Weight::One(), Star(w));
      } else {
        EXPECT_EQ(Weight(-inf), Star(w));
      }
    }
  }
}

TEST(StarTest, Tropical) {
  TestTropical<float>();
  TestTropical<double>();
}

template <class T>
inline void ExpectApproxEq(T x, T y);

template <>
inline void ExpectApproxEq<float>(float x, float y) {
  EXPECT_FLOAT_EQ(x, y);
}

template <>
inline void ExpectApproxEq<double>(double x, double y) {
  EXPECT_DOUBLE_EQ(x, y);
}

template <class T>
inline void ExpectRelEq(T x, T y);

template <>
inline void ExpectRelEq<float>(float x, float y) {
  EXPECT_LT(std::abs((x - y) / x), 1e-6);
}

template <>
inline void ExpectRelEq<double>(double x, double y) {
  EXPECT_LT(std::abs((x - y) / x), 1e-14);
}

template <class T>
void TestLog() {
  typedef fst::LogWeightTpl<T> Weight;
  // Test real probabilities very close to and including 0:
  T p = 0;
  for (int n = 0; n < 1000; ++n) {
    Weight w = -std::log(p);
    Weight star = Star(w);
    EXPECT_EQ((p != 0), std::isfinite(w.Value()));
    EXPECT_GT(w.Value(), 0);
    EXPECT_TRUE(w.Member());
    EXPECT_TRUE(star.Member());
    ExpectApproxEq<T>(1.0 / (1.0 - p), std::exp(-star.Value()));
    ExpectApproxEq<T>(std::log1p(-p), star.Value());
    p = std::nextafter(p, static_cast<T>(inf));
    EXPECT_GT(p, 0);
  }
  // Test real probabilities close to zero via big log probabilities:
  for (T lp : {745.0f, 745.1332191f, 745.1332192f, 746.0f}) {
    p = std::exp(-lp);
    Weight w = lp;
    Weight star = Star(w);
    EXPECT_TRUE(w.Member());
    EXPECT_TRUE(star.Member());
    ExpectApproxEq<T>(1.0 / (1.0 - p), std::exp(-star.Value()));
    ExpectApproxEq<T>(std::log1p(-p), star.Value());
  }
  // Test real probabilities between 0 and 1 but far away from either end:
  static constexpr float kRealProbs[] = {
    0.0156250F, 0.03125F, 0.0625F, 0.125F, 0.1F, 0.2F, 0.25F, 0.3F, 0.4F, 0.5F,
    0.6F, 0.7F, 0.75F, 0.8F, 0.875F, 0.9F, 0.9375F, 0.96875F, 0.984375F,
  };
  for (T p : kRealProbs) {
    Weight w = -std::log(p);
    Weight star = Star(w);
    EXPECT_GT(w.Value(), 0);
    EXPECT_TRUE(w.Member());
    EXPECT_TRUE(star.Member());
    ExpectRelEq<T>(1.0 / (1.0 - p), std::exp(-star.Value()));
    ExpectApproxEq<T>(std::log1p(-p), star.Value());
  }
  p = 1;
  // Test real probabilities very close to but just below 1:
  for (int n = 0; n < 1000; ++n) {
    p = std::nextafter(p, static_cast<T>(-inf));
    Weight w = -std::log(p);
    Weight star = Star(w);
    EXPECT_LT(p, 1);
    EXPECT_GT(w.Value(), 0);
    EXPECT_TRUE(w.Member());
    EXPECT_TRUE(star.Member());
    ExpectRelEq<T>(1.0 / (1.0 - p), std::exp(-star.Value()));
    ExpectApproxEq<T>(std::log1p(-p), star.Value());
  }
  // Test real probability of exactly 1:
  {
    Weight w = -std::log(1);
    Weight star = Star(w);
    EXPECT_EQ(0, w.Value());
    EXPECT_EQ(Weight::One(), w);
    EXPECT_TRUE(w.Member());
    EXPECT_FALSE(star.Member());
  }
  // Test real values above 1:
  p = 1;
  for (int n = 0; n < 1000; ++n) {
    p = std::nextafter(p, static_cast<T>(inf));
    Weight w = -std::log(p);
    Weight star = Star(w);
    EXPECT_GT(p, 1);
    EXPECT_LT(w.Value(), 0);
    EXPECT_TRUE(w.Member());
    EXPECT_FALSE(star.Member());
  }
}

TEST(StarTest, Log) {
  TestLog<float>();
  TestLog<double>();
}

template <class T>
void TestStar3() {
  typedef festus::RealWeightTpl<T> Weight;
  static constexpr float kWeights[] = {
    0.0f, 1.0f, -1.0f, 0.5f, -0.5f, 2.0f, -2.0f
  };
  for (T x : kWeights) {
    Weight w = x;
    Weight s3 = Star(Star(Star(w)));
    ExpectApproxEq<T>(x, s3.Value());
  }
}

TEST(RealWeightTest, Star) {
  TestStar3<float>();
  TestStar3<double>();
}

}  // namespace
