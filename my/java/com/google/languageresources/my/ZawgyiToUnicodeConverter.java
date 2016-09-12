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

import java.util.logging.Logger;
import java.util.regex.Pattern;

/** Conversion from Zawgyi font encoding of Burmese to Unicode 5.1 encoding. */
public final class ZawgyiToUnicodeConverter {
  private final Logger logger =
      Logger.getLogger("com.google.languageresources.my.ZawgyiToUnicodeConverter");

  private final boolean throwOnError;

  private void unexpectedCodePoint(int c, GraphemeComposer gc, StringBuilder unicode) {
    gc.flushTo(unicode);
    unicode.append('\uFFFD');
    String msg = "Unexpected Zawgyi codepoint: " + Integer.toHexString(c);
    logger.warning(msg);
    if (throwOnError) {
      throw new Error(msg);
    }
  }

  /** Constructs a {@code ZawgyiToUnicodeConverter} which handles errors silently. */
  public ZawgyiToUnicodeConverter() {
    this(false);
  }

  /**
   * Constructs a {@code ZawgyiToUnicodeConverter} with the specified error handling behavior.
   *
   * @param throwOnError determines if a runtime error should be thrown when an error occurs
   */
  public ZawgyiToUnicodeConverter(boolean throwOnError) {
    this.throwOnError = throwOnError;
  }

  private static final Pattern SPACE_BEFORE_DOT_BELOW =
      //                 SPACES    ZAWGYI DOT BELOW
      Pattern.compile("\\p{Zs}+([\u1037\u1094\u1095])");

  private static final Pattern ZERO_AFTER_ALPHABETIC =
      //                   LETTERS, MARKS, SYMBOLS     ZERO
      Pattern.compile("([\u1000-\u103F\u104C-\u1059])\u1040");

  private static final Pattern ZERO_BEFORE_LETTER_WITH_ASAT =
      Pattern.compile(
          // ZERO    LETTERS                           VOWELS+DOT   MEDIALS        ASAT
          "\u1040([\u1000-\u1021\u103F\u1050-\u1055][\u102D-\u1037\u103B-\u103E]*\u103A)");

  private static final Pattern FOUR_LAGAUN = Pattern.compile("\u1044\u1004\u103A\u1038");

  /**
   * Converts a Zawgyi codepoint sequence into a Unicode string.
   *
   * @param zawgyi the Zawgyi codepoint sequence to be converted
   * @return the conversion result as a proper Unicode string
   */
  public String convert(CharSequence zawgyi) {
    String z = SPACE_BEFORE_DOT_BELOW.matcher(zawgyi).replaceAll("$1");
    StringBuilder b = new StringBuilder();
    convertTo(z, b);
    String u = ZERO_AFTER_ALPHABETIC.matcher(b).replaceAll("$1\u101D");
    u = ZERO_BEFORE_LETTER_WITH_ASAT.matcher(u).replaceAll("\u101D$1");
    u = FOUR_LAGAUN.matcher(u).replaceAll("\u104E\u1004\u103A\u1038");
    return u;
  }

  private void convertTo(CharSequence zawgyi, StringBuilder unicode) {
    GraphemeComposer gc = new GraphemeComposer("\u1031\u103C\u1040", throwOnError);
    for (int i = 0; i < zawgyi.length(); i++) {
      char c = zawgyi.charAt(i);
      switch (c) {
        case 0x1022: unexpectedCodePoint(c, gc, unicode); break;

        case 0x1028: unexpectedCodePoint(c, gc, unicode); break;

        case 0x1033: gc.add('\u102F', unicode); break;
        case 0x1034: gc.add('\u1030', unicode); break;
        case 0x1035: unexpectedCodePoint(c, gc, unicode); break;

        case 0x1039: gc.add('\u103A', unicode); break;
        case 0x103A: gc.add('\u103B', unicode); break;
        case 0x103B: gc.add('\u103C', unicode); break;
        case 0x103C: gc.add('\u103D', unicode); break;
        case 0x103D: gc.add('\u103E', unicode); break;
        case 0x103E:
        case 0x103F: unexpectedCodePoint(c, gc, unicode); break;

        case 0x104E: gc.add("\u104E\u1004\u103A\u1038", unicode); break;

        case 0x1050:
        case 0x1051:
        case 0x1052:
        case 0x1053:
        case 0x1054:
        case 0x1055:
        case 0x1056:
        case 0x1057:
        case 0x1058:
        case 0x1059: unexpectedCodePoint(c, gc, unicode); break;
        case 0x105A: gc.add("\u102B\u103A", unicode); break;
        case 0x105B:
        case 0x105C:
        case 0x105D:
        case 0x105E:
        case 0x105F: unexpectedCodePoint(c, gc, unicode); break;
        case 0x1060: gc.setStacked('\u1000'); break;
        case 0x1061: gc.setStacked('\u1001'); break;
        case 0x1062: gc.setStacked('\u1002'); break;
        case 0x1063: gc.setStacked('\u1003'); break;
        case 0x1064: gc.setKinzi('\u1004'); break;
        case 0x1065: gc.setStacked('\u1005'); break;
        case 0x1066:
        case 0x1067: gc.setStacked('\u1006'); break;
        case 0x1068: gc.setStacked('\u1007'); break;
        case 0x1069: gc.setStacked('\u1008'); break;
        case 0x106A: gc.add('\u1009', unicode); break;
        case 0x106B: gc.add('\u100A', unicode); break;
        case 0x106C: gc.setStacked('\u100B'); break;
        case 0x106D: gc.setStacked('\u100C'); break;
        case 0x106E: gc.add('\u100D', unicode).setStacked('\u100D'); break;
        case 0x106F: gc.add('\u100E', unicode).setStacked('\u100D'); break;
        case 0x1070: gc.setStacked('\u100F'); break;
        case 0x1071:
        case 0x1072: gc.setStacked('\u1010'); break;
        case 0x1073:
        case 0x1074: gc.setStacked('\u1011'); break;
        case 0x1075: gc.setStacked('\u1012'); break;
        case 0x1076: gc.setStacked('\u1013'); break;
        case 0x1077: gc.setStacked('\u1014'); break;
        case 0x1078: gc.setStacked('\u1015'); break;
        case 0x1079: gc.setStacked('\u1016'); break;
        case 0x107A: gc.setStacked('\u1017'); break;
        case 0x107B: gc.setStacked('\u1018'); break;
        case 0x107C: gc.setStacked('\u1019'); break;
        case 0x107D: gc.add('\u103B', unicode); break;
        case 0x107E:
        case 0x107F:
        case 0x1080:
        case 0x1081:
        case 0x1082:
        case 0x1083:
        case 0x1084: gc.add('\u103C', unicode); break;
        case 0x1085: gc.setStacked('\u101C'); break;
        case 0x1086: gc.add('\u103F', unicode); break;
        case 0x1087: gc.add('\u103E', unicode); break;
        case 0x1088: gc.add("\u103E\u102F", unicode); break;
        case 0x1089: gc.add("\u103E\u1030", unicode); break;
        case 0x108A: gc.add("\u103D\u103E", unicode); break;
        case 0x108B: gc.setKinzi('\u1004').add('\u102D', unicode); break;
        case 0x108C: gc.setKinzi('\u1004').add('\u102E', unicode); break;
        case 0x108D: gc.setKinzi('\u1004').add('\u1036', unicode); break;
        case 0x108E: gc.add("\u102D\u1036", unicode); break;
        case 0x108F: gc.add('\u1014', unicode); break;
        case 0x1090: gc.add('\u101B', unicode); break;
        case 0x1091: gc.add('\u100F', unicode).setStacked('\u100D'); break;
        case 0x1092: gc.add('\u100B', unicode).setStacked('\u100C'); break;
        case 0x1093: gc.setStacked('\u1018'); break;
        case 0x1094:
        case 0x1095: gc.add('\u1037', unicode); break;
        case 0x1096: gc.setStacked('\u1010').add('\u103D', unicode); break;
        case 0x1097: gc.add('\u100B', unicode).setStacked('\u100B'); break;
        case 0x1098:
        case 0x1099:
        case 0x109A:
        case 0x109B:
        case 0x109C:
        case 0x109D:
        case 0x109E:
        case 0x109F: unexpectedCodePoint(c, gc, unicode); break;
        default: gc.add(c, unicode); break;
      }
    }
    gc.flushTo(unicode);
  }
}
