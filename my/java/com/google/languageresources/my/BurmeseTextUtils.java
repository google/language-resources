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
import java.text.Normalizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/** Utilities for working with Burmese text. */
class BurmeseTextUtils {

  /** Helper interface for producing Unicode text. */
  public static interface Unicodifier {
    public String toUnicode(String original);
  }

  /** {@code Unicodifier} that takes Unicode text as input and canonicalizes it. */
  public static final Unicodifier FROM_UNICODE =
      new Unicodifier() {
        @Override
        public String toUnicode(String original) {
          GraphemeComposer composer = new GraphemeComposer();
          StringBuilder builder = new StringBuilder();
          composer.add(original, builder).flushTo(builder);
          return builder.toString();
        }
      };

  /** {@code Unicodifier} that takes Zawgyi text as input and converts it to Unicode. */
  public static final Unicodifier FROM_ZAWGYI =
      new Unicodifier() {
        private final ZawgyiToUnicodeConverter converter = new ZawgyiToUnicodeConverter();

        @Override
        public String toUnicode(String original) {
          return converter.convert(original);
        }
      };

  /** Pattern that matches contiguous spans of Myanmar text with interspersed whitespace. */
  private static final Pattern MYANMAR_TEXT =
      Pattern.compile("[\u1000-\u109F]+(\\p{Zs}+[\u1000-\u109F]+)*");

  /**
   * Converts Burmese text to Unicode using the provided {@code Unicodifier}.
   *
   * <p>Applies the given {@code Unicodifier} to each contiguous span of Burmese text in the
   * provided {@code String}. Leaves other text spans unchanged.
   *
   * @param unifier the {@code Unicodifier} that will be applied to the Maynmar spans
   * @param text text to be converted to Unicode
   * @return Unicode text resulting from the conversion described above
   */
  public static String toUnicode(Unicodifier unifier, String text) {
    StringBuilder builder = new StringBuilder();
    int prev = 0;
    Matcher matcher = MYANMAR_TEXT.matcher(text);
    while (matcher.find()) {
      String unicode = unifier.toUnicode(matcher.group());
      if (!Normalizer.isNormalized(unicode, Normalizer.Form.NFC)) {
        throw new Error("Replacement string \"" + unicode + "\" is not in NFC");
      }
      builder.append(text, prev, matcher.start()).append(unicode);
      prev = matcher.end();
    }
    builder.append(text, prev, text.length());
    return builder.toString();
  }

  /**
   * Read-replace-print loop that reads UTF-8 text from stdin, converts it to Unicode using the
   * given {@code Unicodifier}, and writes the result to stdout.
   *
   * @param unifier the {@code Unicodifier} that will be applied to the Maynmar spans
   */
  public static void readReplacePrint(Unicodifier unifier) throws IOException {
    BufferedReader in = new BufferedReader(new InputStreamReader(System.in, UTF_8));
    PrintWriter out = new PrintWriter(new OutputStreamWriter(System.out, UTF_8), true);
    for (String line = in.readLine(); line != null; line = in.readLine()) {
      out.println(toUnicode(unifier, line));
    }
    in.close();
    out.close();
  }
}
