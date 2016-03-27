// festus/real-weight-test.cc
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
// Unit test for semiring of real weights.

#include "festus/real-weight.h"

#include <cstdlib>
#include <ctime>
#include <type_traits>

#include <fst/compat.h>
#include <fst/float-weight.h>
#include <test/weight-tester.h>  // fst/test/weight-tester.h
#include <gtest/gtest.h>

#include "festus/weight-test-lib.h"

DEFINE_int32(seed, -1, "random seed");
DEFINE_int32(repeat, 100000, "number of test repetitions");

namespace {

template <class T>
class RealWeightGenerator_ {
 public:
  typedef festus::RealWeightTpl<T> Weight;

  RealWeightGenerator_(int seed = std::time(0), bool allow_zero = true)
      : allow_zero_(allow_zero) {
    std::srand(seed);
  }

  Weight operator() () const {
    while (true) {
      int n = (std::rand() % (2 * kNumRandomWeights + 1)) - kNumRandomWeights;
      if (n == 0) {
        if (allow_zero_) {
          return Weight::Zero();
        } else {
          continue;
        }
      }
      int d = (std::rand() % kNumRandomWeights) + 1;
      return Weight(n / static_cast<T>(d));
    }
  }

 private:
  // The number of alternative random weights.
  static const int kNumRandomWeights = 5;

  const bool allow_zero_;  // permit Zero() and zero divisors
};

template <class T> const int RealWeightGenerator_<T>::kNumRandomWeights;

template <class T>
void TestTemplatedWeights(int repeat, int seed) {
  RealWeightGenerator_<T> real_generator(seed);
  fst::WeightTester<festus::RealWeightTpl<T>, RealWeightGenerator_<T> >
      real_tester(real_generator);
  real_tester.Test(repeat);
}

TEST(RealWeightTest, RandomWeights) {
  int seed = FLAGS_seed >= 0 ? FLAGS_seed : std::time(0);
  LOG(INFO) << "Seed = " << seed;
  TestTemplatedWeights<float>(FLAGS_repeat, seed);
  TestTemplatedWeights<double>(FLAGS_repeat, seed);
  TestTemplatedWeights<long double>(FLAGS_repeat, seed);
}

TEST(RealWeightTest, TypeName) {
  EXPECT_EQ("real", festus::RealWeight::Type());
  EXPECT_NE(festus::RealWeightTpl<double>::Type(),
            festus::RealWeightTpl<float>::Type());
}

TEST(RealWeightTest, TypeTraits) {
  // These could become compile-time static assertions, but that would risk
  // build breakage instead of test failures.
  EXPECT_EQ(sizeof(float), sizeof(festus::RealWeightTpl<float>));
  EXPECT_EQ(sizeof(double), sizeof(festus::RealWeightTpl<double>));
  EXPECT_EQ(sizeof(long double), sizeof(festus::RealWeightTpl<long double>));

  EXPECT_TRUE(std::is_pod<festus::RealWeightTpl<float>>::value);
  EXPECT_TRUE(std::is_pod<festus::RealWeightTpl<double>>::value);
  EXPECT_TRUE(std::is_pod<festus::RealWeightTpl<long double>>::value);
}

TEST(RealWeightTest, BasicWeights) {
  festus::TestBasicWeights<festus::RealWeightTpl<float>>();
  festus::TestBasicWeights<festus::RealWeightTpl<double>>();
  festus::TestBasicWeights<festus::RealWeightTpl<long double>>();
}

template <typename T>
void TestRealIdentities() {
  typedef festus::RealWeightTpl<T> Weight;
  TestIdentities(Weight::Zero());
  TestIdentities(Weight::One());
  TestIdentities(Weight());
  TestIdentities(Weight(0));
  TestIdentities(Weight(1));
  TestIdentities(Weight(2));
  TestIdentities(Weight(22 / 7.0f));
  TestIdentities(Weight(-0.0f));
  TestIdentities(Weight(-1));
  TestIdentities(Weight(-2));
  TestIdentities(Weight(-1e6f));
  TestIdentities(Plus(Weight(43), Weight(9)));
  TestIdentities(Minus(Weight(43), Weight(9)));
  TestIdentities(Times(Weight(43), Weight(9)));
  TestIdentities(Star(Weight(9)));
}

TEST(RealWeightTest, Identities) {
  TestRealIdentities<float>();
  TestRealIdentities<double>();
  TestRealIdentities<long double>();
}

}  // namespace

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  SET_FLAGS(argv[0], &argc, &argv, true);
  return RUN_ALL_TESTS();
}
