// Copyright 2019 Google LLC. All Rights Reserved.
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

#ifndef FESTUS_WEIGHT_CONVERT_H__
#define FESTUS_WEIGHT_CONVERT_H__

#include <fst/arc-map.h>
#include <fst/float-weight.h>
#include <fst/fst.h>
#include <fst/mutable-fst.h>
#include <fst/weight.h>

namespace fst {

template <>
struct WeightConvert<LogWeightTpl<double>, TropicalWeightTpl<double>> {
  constexpr TropicalWeightTpl<double> operator()(
      const LogWeightTpl<double> &w) const {
    return w.Value();
  }
};

template <>
struct WeightConvert<TropicalWeightTpl<double>, LogWeightTpl<double>> {
  constexpr LogWeightTpl<double> operator()(
      const TropicalWeightTpl<double> &w) const {
    return w.Value();
  }
};

}  // namespace fst

namespace festus {

template <class FromArc, class ToArc>
void ConvertWeight(const fst::Fst<FromArc> &from_fst,
                   fst::MutableFst<ToArc> *to_fst) {
  fst::ArcMap(from_fst, to_fst, fst::WeightConvertMapper<FromArc, ToArc>());
}

}  // namespace festus

#endif  // FESTUS_WEIGHT_CONVERT_H__
