// festus/label-maker.h
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
// Conversion between symbols and numeric labels.

#ifndef FESTUS_LABEL_MAKER_H__
#define FESTUS_LABEL_MAKER_H__

#include <vector>

#include <fst/compat.h>
#include <fst/symbol-table.h>

#include "festus/string-util.h"

namespace festus {

// Abstract converter between C++ byte strings and FST label sequences.
class LabelMaker {
 public:
  typedef std::vector<int> Labels;

  virtual ~LabelMaker() = default;

  virtual const fst::SymbolTable *Symbols() const { return nullptr; }

  virtual bool StringToLabels(const StringPiece str, Labels *labels) const = 0;
  virtual bool LabelsToString(const Labels &labels, string *str) const = 0;

  template <class CompactFstType>
  bool StringToCompactFst(const StringPiece str, CompactFstType *fst) {
    Labels labels;
    if (!StringToLabels(str, &labels)) return false;
    fst->SetCompactElements(labels.begin(), labels.end());
    return true;
  }
};

// Converter that turns a C++ byte string into an FST label sequence where
// bytes (viewed as unsigned integers) correspond one-for-one to labels.
class ByteLabelMaker : public LabelMaker {
 public:
  ByteLabelMaker() = default;

  bool StringToLabels(const StringPiece str, Labels *labels) const override;
  bool LabelsToString(const Labels &labels, string *str) const override;
};

// Converter between UTF-8 byte strings and sequences of Unicode codepoints.
// Strings must be structurally valid UTF-8. Labels represent UTF-32 codepoints.
class UnicodeLabelMaker : public LabelMaker {
 public:
  UnicodeLabelMaker() = default;

  bool StringToLabels(const StringPiece str, Labels *labels) const override;
  bool LabelsToString(const Labels &labels, string *str) const override;
};

// Converter that splits a byte string into symbols and looks up the label
// values in the provided symbol table.
class SymbolLabelMaker : public LabelMaker {
 public:
  // TODO: Add special handling of "" as delimiter.
  SymbolLabelMaker(const fst::SymbolTable *symbols, string delimiters);
  ~SymbolLabelMaker() override;

  const fst::SymbolTable *Symbols() const override { return symbols_; }

  bool StringToLabels(const StringPiece str, Labels *labels) const override;
  bool LabelsToString(const Labels &labels, string *str) const override;

 private:
  SymbolLabelMaker() = delete;

  const fst::SymbolTable *const symbols_;
  const string delimiters_;
};

}  // namespace festus

#endif  // FESTUS_LABEL_MAKER_H__
