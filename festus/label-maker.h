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
// Copyright 2015 Google, Inc.
// Author: mjansche@google.com (Martin Jansche)
//
// \file
// Conversion between symbols and numeric labels.

#ifndef FESTUS_LABEL_MAKER_H__
#define FESTUS_LABEL_MAKER_H__

#include <vector>

#include <fst/compat.h>
#include <fst/symbol-table.h>

namespace festus {

// TODO: Rewrite in terms of google::protobuf::StringPiece.
std::vector<string> Split(const string &str, const string &delimiters);

class LabelMaker {
 public:
  typedef std::vector<int> Labels;

  virtual ~LabelMaker() = default;

  virtual const fst::SymbolTable *Symbols() const = 0;

  virtual bool StringToLabels(const string &str, Labels *labels) const = 0;
  virtual bool LabelsToString(const Labels &labels, string *str) const = 0;
};

class ByteLabelMaker : public LabelMaker {
 public:
  ByteLabelMaker() = default;

  const fst::SymbolTable *Symbols() const override { return nullptr; }

  bool StringToLabels(const string &str, Labels *labels) const override;
  bool LabelsToString(const Labels &labels, string *str) const override;
};

class SymbolLabelMaker : public LabelMaker {
 public:
  // TODO: Generalize delimiter to const char *; add special handling
  // of "" as delimiter.
  SymbolLabelMaker(const fst::SymbolTable *symbols,
                   char delimiter);
  ~SymbolLabelMaker() override;

  const fst::SymbolTable *Symbols() const override { return symbols_; }

  bool StringToLabels(const string &str, Labels *labels) const override;
  bool LabelsToString(const Labels &labels, string *str) const override;

 private:
  SymbolLabelMaker() = delete;

  const fst::SymbolTable *const symbols_;
  const char delimiter_;
};

}  // namespace festus

#endif  // FESTUS_LABEL_MAKER_H__
