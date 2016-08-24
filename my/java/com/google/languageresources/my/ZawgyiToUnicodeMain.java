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

import static java.nio.charset.StandardCharsets.UTF_8;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;

/**
 * Command-line interface for Zawgyi-to-Unicode conversion.
 *
 * <p>Reads UTF-8 text using Zawgyi codepoints from stdin and writes UTF-8 text follwing the
 * Unicodde 5.1 encoding model to stdout.
 */
class ZawgyiToUnicodeMain {
  public static void main(String[] args) throws IOException {
    BufferedReader in = new BufferedReader(new InputStreamReader(System.in, UTF_8));
    PrintWriter out = new PrintWriter(new OutputStreamWriter(System.out, UTF_8), true);
    ZawgyiToUnicodeConverter converter = new ZawgyiToUnicodeConverter(false);
    for (String line = in.readLine(); line != null; line = in.readLine()) {
      String unicode = converter.convert(line);
      out.println(unicode);
    }
    in.close();
    out.close();
  }
}
