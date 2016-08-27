// Copyright 2016 Google, Inc. All Rights Reserved.
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

package com.google.languageresources.my;

/**
 * Command-line interface for Zawgyi-to-Unicode conversion.
 *
 * <p>Reads UTF-8 text using Zawgyi codepoints from stdin and writes UTF-8 text follwing the
 * Unicodde 5.1 encoding model to stdout.
 */
class ZawgyiToUnicodeMain {
  public static void main(String[] args) throws java.io.IOException {
    BurmeseTextUtils.readReplacePrint(BurmeseTextUtils.FROM_ZAWGYI);
  }
}
