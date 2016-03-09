// festus/types.h
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
// Type helpers for OpenFst.

#ifndef FESTUS_TYPES_H__
#define FESTUS_TYPES_H__

namespace festus {

template <class F> struct FstType             { typedef F Type; };
template <class F> struct FstType<F &>        { typedef F Type; };
template <class F> struct FstType<F *>        { typedef F Type; };
template <class F> struct FstType<F &&>       { typedef F Type; };
template <class F> struct FstType<const F &>  { typedef F Type; };
template <class F> struct FstType<const F *>  { typedef F Type; };

#define FST_TYPE(x)      ::festus::FstType<decltype(x)>::Type
#define ARC_TYPE(x)      FST_TYPE(x)::Arc
#define WEIGHT_TYPE(x)   FST_TYPE(x)::Weight
#define STATE_ID_TYPE(x) FST_TYPE(x)::StateId
#define LABEL_TYPE(x)    ARC_TYPE(x)::Label

}  // namespace festus

#endif  // FESTUS_TYPES_H__
