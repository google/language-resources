// Copyright 2018 Google LLC. All Rights Reserved.
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

// \file
// Negative binomial (Pascal) length distribution.

#ifndef FESTUS_NBINOM_H__
#define FESTUS_NBINOM_H__

#include <fst/compat.h>
#include <fst/mutable-fst.h>

namespace festus {

// Sets the argument 'fst' to an FST with a negative binomial (Pascal) length
// distribution.
//
// The implicit parametrization assumed here uses (size, prob) with size >= 0
// and 0 <= prob <= 1. A random variable X ~ nbinom(size, prob) represents the
// number of failures in a sequences of i.i.d. Bernoulli trials with success
// probability 'prob' until 'size' successes have been reached. Equivalently,
// X is also the sum of 'size' i.i.d. 0-based geometric distributions with
// parameter 'prob'.
//
// A "failure" in a Bernoulli trial corresponds to a substantive arc (with the
// given label) in the FST, and a "success" corresponds to an epsilon-arc. By
// this construction every successful path in the FST contains exactly 'size'
// epsilon-arcs. Since epsilon-arcs do not contribute to the string value of a
// path, the length of a randomly generated string has an nbinom(size, prob)
// distribution.
//
// The Bernoulli success probability is passed in as a pair of weights
// 'odds_for' : 'odds_against' expressing the odds of success. This is for
// technical reasons, since the OpenFst weight library does not provide a
// Minus() (i.e. ring) operation on weights, but does provide a Divide
// (i.e. field) operation.
template <class A>
void NegativeBinomial(fst::MutableFst<A> *fst,
                      int size,
                      typename A::Weight odds_for,
                      typename A::Weight odds_against,
                      typename A::Label label=1) {
  typedef typename A::Weight Weight;
  const Weight total = Plus(odds_for, odds_against);
  const Weight prob_success = Divide(odds_for, total);
  const Weight prob_failure = Divide(odds_against, total);
  fst->DeleteStates();
  fst->ReserveStates(size + 1);
  auto q = fst->AddState();
  fst->SetStart(q);
  for (int i = 0; i < size; ++i) {
    auto r = fst->AddState();
    fst->AddArc(q, A(label, label, prob_failure, q));
    fst->AddArc(q, A(0, 0, prob_success, r));
    q = r;
  }
  fst->SetFinal(q, Weight::One());
}

}  // namespace festus

#endif  // FESTUS_NBINOM_H__
