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
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.text.Normalizer;
import org.junit.Test;

public class ZawgyiToUnicodeConverterTest {

  private static final ZawgyiToUnicodeConverter CONVERTER = new ZawgyiToUnicodeConverter();

  private void checkConverter(String zawgyi, String expected) {
    String actual = CONVERTER.convert(zawgyi);
    assertTrue(Normalizer.isNormalized(actual, Normalizer.Form.NFC));
    assertEquals(expected, actual);
  }

  @Test
  public void testIdentity() {
    checkConverter("\u1015\u102B", "\u1015\u102B");
  }

  @Test
  public void testSimple() {
    checkConverter("\u1000\u1039", "\u1000\u103A");
  }

  @Test
  public void testEmbeddedSpace() {
    checkConverter("\u1000 \u1037\u1001", "\u1000\u1037\u1001");
  }

  @Test
  public void testZeroToWa() {
    checkConverter("\u101C\u102F\u1036\u1038\u1040", "\u101C\u102F\u1036\u1038\u101D");
    checkConverter("\u1040\u1004\u1039\u1038", "\u101D\u1004\u103A\u1038");
  }

  @Test
  public void testExhaustive() throws IOException {
    InputStream istream = ClassLoader.getSystemResourceAsStream("my/zawgyi_unicode_test.tsv");
    assertNotNull(istream);
    BufferedReader reader = new BufferedReader(new InputStreamReader(istream, UTF_8));
    boolean header = true;
    for (String line = reader.readLine(); line != null; line = reader.readLine()) {
      if (header) {
        header = false;
        continue;
      }
      String[] fields = line.split("\t");
      checkConverter(fields[1], fields[2]);
    }
    reader.close();
  }
}
