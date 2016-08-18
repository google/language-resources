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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import java.text.Normalizer;
import org.junit.Test;

public class GraphemeComposerTest {

  private final GraphemeComposer composer = new GraphemeComposer("\u1031\u103C");

  private void checkComposer(String toAdd, String expected) {
    composer.clear();
    StringBuilder builder = new StringBuilder();
    composer.add(toAdd, builder).flushTo(builder);
    String actual = builder.toString();
    assertTrue(Normalizer.isNormalized(actual, Normalizer.Form.NFC));
    assertEquals(expected, actual);
  }

  @Test
  public void testClusterStart() {
    checkComposer("\u1000\u1031\u103C\u1001", "\u1000\u1001\u103C\u1031");
    checkComposer("\u1000\u103C\u1031\u1001", "\u1000\u1001\u103C\u1031");
  }

  @Test
  public void testZero() {
    checkComposer("\u1031\u1040", "\u101D\u1031");
  }

  @Test
  public void testDotAsat() {
    checkComposer("\u1000\u103A\u1037", "\u1000\u1037\u103A");
  }
}
