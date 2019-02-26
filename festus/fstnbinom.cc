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
// Makes an FST with a negative binomial (Pascal) length distribution.

#include <cmath>
#include <cstdlib>
#include <cstring>

#include <fst/compat.h>
#include <fst/fstlib.h>

#include "festus/nbinom.h"

const char kUsage[] =
    R"(Makes an FST with a negative binomial length distribution.

Usage:
  nbinom size mu [out.fst]
)";

int main(int argc, char *argv[]) {
  SET_FLAGS(kUsage, &argc, &argv, true);
  if (argc != 3 && argc != 4) {
    ShowUsage();
    return 2;
  }

  const int size = std::strtol(argv[1], nullptr, 0);
  const double mu = std::strtod(argv[2], nullptr);
  // The value extra_poisson is a multiplicative term which determines the
  // relative extra-Poisson variance. The variance of the negative binomial
  // distribution with parameters size and mu is mu * extra_poisson, whereas
  // the variance of a Poisson distribution with the same mean mu is simply
  // mu. For a fixed value of mu, the negative binomial distribution
  // approaches the Poisson distribution with mean mu as size approaches
  // infinity, and hence extra_poisson approaches 1.
  const double extra_poisson = 1 + mu / size;

  std::cerr << "size: " << size << std::endl;
  std::cerr << "prob: "
            << size << "/" << "(" << size << "+" << mu << ") = "
            << size / (size + mu) << std::endl;
  std::cerr << "mean: " << mu << std::endl;
  std::cerr << "var:  "
            << mu << "*(1+" << mu << "/" << size << ") = "
            << mu << "*" << extra_poisson << " = "
            << mu * extra_poisson << std::endl;

  // Since prob == size / (size + mu), odds are size : mu.
  const fst::Log64Weight odds_for = -std::log(size);
  const fst::Log64Weight odds_against = -std::log(mu);

  fst::VectorFst<fst::Log64Arc> fst;
  festus::NegativeBinomial(&fst, size, odds_for, odds_against);

  string out = (argc > 3 && std::strcmp(argv[3], "-") != 0) ? argv[3] : "";
  fst.Write(out);

  return 0;
}
