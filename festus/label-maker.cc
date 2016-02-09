// festus/label-maker.cc
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

#include "festus/label-maker.h"

#include <cstddef>
#include <utility>
#include <vector>

#include <fst/compat.h>
#include <fst/icu.h>
#include <fst/symbol-table.h>

#include "festus/string-util.h"

namespace festus {

bool ByteLabelMaker::StringToLabels(const StringPiece str,
                                    Labels *labels) const {
  labels->clear();
  labels->reserve(str.size());
  for (const auto byte : str) {
    labels->emplace_back(static_cast<unsigned char>(byte));
  }
  return true;
}

bool ByteLabelMaker::LabelsToString(const Labels &labels, string *str) const {
  str->clear();
  str->reserve(labels.size());
  for (const auto label : labels) {
    if (label < 0 || label > 255) {
      LOG(ERROR) << "Invalid label in ByteLabelMaker: " << label;
      return false;
    }
    str->push_back(static_cast<char>(label));
  }
  return true;
}

bool UnicodeLabelMaker::StringToLabels(const StringPiece str,
                                       Labels *labels) const {
  labels->clear();
  if (!IsStructurallyValidUTF8(str)) {
    LOG(WARNING) << "String is not structurally valid UTF-8: " << str;
    return false;
  }
  return fst::UTF8StringToLabels(str.ToString(), labels);
}

bool UnicodeLabelMaker::LabelsToString(const Labels &labels,
                                       string *str) const {
  return fst::LabelsToUTF8String(labels, str);
}

SymbolLabelMaker::SymbolLabelMaker(const fst::SymbolTable *symbols,
                                   string delimiters)
    : symbols_(symbols ? symbols->Copy() : nullptr),
      delimiters_(std::move(delimiters)) {
}

SymbolLabelMaker::~SymbolLabelMaker() {
  delete symbols_;
}

bool SymbolLabelMaker::StringToLabels(const StringPiece str,
                                      Labels *labels) const {
  const auto symbols = Split(str, delimiters_);
  labels->clear();
  labels->reserve(symbols.size());
  for (const auto &symbol : symbols) {
    auto label = symbols_->Find(symbol.ToString());
    if (label == fst::SymbolTable::kNoSymbol) {
      LOG(ERROR) << "Unknown symbol in SymbolLabelMaker: " << symbol;
      return false;
    }
    const int ilabel = static_cast<int>(label);
    if (ilabel != label) {
      LOG(ERROR) << "Label too large in SymbolLabelMaker: " << label;
      return false;
    }
    VLOG(1) << "Found label " << ilabel << " for symbol \"" << symbol << "\"";
    labels->emplace_back(ilabel);
  }
  return true;
}

bool SymbolLabelMaker::LabelsToString(const Labels &labels, string *str) const {
  if (labels.empty()) {
    str->clear();
    return true;
  }
  std::vector<string> symbols;
  symbols.reserve(labels.size());
  std::size_t length = 0;
  for (const auto label : labels) {
    string symbol = symbols_->Find(label);
    if (symbol.empty()) {
      LOG(ERROR) << "Unknown label in SymbolLabelMaker: " << label;
      return false;
    }
    length += symbol.size();
    symbols.emplace_back(std::move(symbol));
  }
  DCHECK_GE(symbols.size(), 1);
  length += symbols.size() - 1;
  str->clear();
  str->reserve(length);
  str->append(symbols[0]);
  for (std::size_t i = 1; i < symbols.size(); ++i) {
    if (!delimiters_.empty()) {
      str->push_back(delimiters_[0]);
    }
    str->append(symbols[i]);
  }
  return true;
}

}  // namespace festus
