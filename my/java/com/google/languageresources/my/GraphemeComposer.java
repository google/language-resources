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

import java.util.ArrayList;

/**
 * A {@code GraphemeComposer} builds grapheme clusters from character sequences.
 *
 * <p>A grapheme cluster (represented by the private class {@link GraphemeCluster}) is a vertical
 * stack consisting of a main letter and optional associated modifiers. The definition of a grapheme
 * cluster is mostly consistent with description in the <a
 * href="http://unicode.org/notes/tn11/">Unicode Technical Note #11</a> (Representing Myanmar in
 * Unicode: Details and Examples, Version 4, 2012-12-13). The version here differs from UTN 11 in
 * that it is restricted to character combinations that are used for writing the Burmese language.
 *
 * <p>A {@code GraphemeComposer} is used to dynamically assemble grapheme clusters from their basic
 * character components. It supports some editing functionality for use by input methods.
 */
public class GraphemeComposer {
  private final String clusterStartCharacters;
  private final GraphemeCluster graphemeCluster;
  private final ArrayList<Character> sequence = new ArrayList<Character>(16);

  /**
   * Constructs a {@code GraphemeComposer} with no additional cluster start characters and which
   * handles errors silently.
   *
   * @see #GraphemeComposer(String, boolean)
   */
  public GraphemeComposer() {
    this("", false);
  }

  /**
   * Constructs a {@code GraphemeComposer} with the specified cluster start characters and which
   * handles errors silently.
   *
   * @param clusterStartCharacters characters that start a new cluster
   * @see #GraphemeComposer(String, boolean)
   */
  public GraphemeComposer(String clusterStartCharacters) {
    this(clusterStartCharacters, false);
  }

  /**
   * Constructs a {@code GraphemeComposer} that treats each character in {@code
   * clusterStartCharacters} as the start of a grapheme cluster and with the specified error
   * handling behavior.
   *
   * <p>By default only independent characters (letters, symbols, punctuation marks) can start a
   * grapheme cluster, whereas modifier characters (in Unicode general category "M") cannot.
   * However, the customary behavior of certain characters can differ from this default. For
   * example, in input methods the vowel 'e' (U+1031) is often treated as if it was an independent
   * letter occurring before the consonant it is affiliated with. To support this behavior, clients
   * can create a {@code GraphemeComposer} with {@code new GraphemeComposer("\u1031")}.
   *
   * <p>If {@code throwOnError} is {@code false}, the newly constructed object handles errors
   * silently; otherwise it throws {@code Error}s.
   *
   * @param clusterStartCharacters characters that start a new cluster
   * @param throwOnError determines if a runtime error should be thrown when an error occurs
   * @see #isStartOfCluster(int)
   */
  public GraphemeComposer(String clusterStartCharacters, boolean throwOnError) {
    this.clusterStartCharacters = clusterStartCharacters;
    this.graphemeCluster = new GraphemeCluster(throwOnError);
  }

  /**
   * Resets the composer to an empty state.
   *
   * <p>Any currently pending edits are discarded.
   */
  public void clear() {
    sequence.clear();
    graphemeCluster.clear();
  }

  /**
   * Commits the currently pending additions to a {@code String} and resets the composer to an empty
   * state.
   *
   * <p>Retrieves the currently pending grapheme cluster as a {@code String}, then resets the
   * composer to an empty state via {@link #clear()}.
   *
   * @return the current grapheme cluster
   */
  private String flush() {
    String s = graphemeCluster.toString();
    clear();
    return s;
  }

  /**
   * Commits the currently pending additions to a {@code StringBuilder} and resets the composer to
   * an empty state.
   *
   * <p>Appends the currently pending grapheme cluster to the provided {@code StringBuilder}, then
   * resets the composer to an empty state via {@link #clear()}.
   *
   * @param builder the {@code StringBuilder} to which the currently pending cluster get appended
   * @return {@code this} for easy chaining of method invocations
   */
  public GraphemeComposer flushTo(StringBuilder builder) {
    graphemeCluster.appendTo(builder);
    clear();
    return this;
  }

  /**
   * Determines if the currently pending grapheme cluster is empty.
   *
   * @return {@code true} iff there are no currently pending additions
   */
  public boolean isEmpty() {
    return sequence.isEmpty();
  }

  /**
   * Sets the <i>kinzi</i> slot of the current grapheme cluster.
   *
   * <p><b>Does not update the edit sequence; use with caution!</b>
   *
   * @param c the character (Unicode code point) to be set as <i>kinzi</i>
   * @return {@code this} for easy chaining of method invocations
   */
  GraphemeComposer setKinzi(int c) {
    // Does not update sequence. Use with caution.
    graphemeCluster.setKinzi(c);
    return this;
  }

  /**
   * Sets the stacked consonant slot of the current grapheme cluster.
   *
   * <p><b>Does not update the edit sequence; use with caution!</b>
   *
   * @param c the character (Unicode code point) to be set as the stacked consonant
   * @return {@code this} for easy chaining of method invocations
   */
  GraphemeComposer setStacked(int c) {
    graphemeCluster.setStacked(c);
    return this;
  }

  /**
   * Determines if the given codepoint starts a new grapheme cluster.
   *
   * @param c the character (Unicode code point) to be tested
   * @return {@code true} iff the character (Unicode code point) can start a new grapheme cluster
   */
  public boolean isStartOfCluster(int c) {
    return (0x1000 <= c && c <= 0x102A)
        || (0x103F <= c && c <= 0x1055)
        || clusterStartCharacters.indexOf(c) >= 0;
  }

  /**
   * Invokes {@link #add(int, StringBuilder)} for each character from a given character sequence.
   *
   * @param chars the sequence of characters to be added
   * @param builder the {@code StringBuilder} to which any completed clusters get appended
   * @return {@code this} for easy chaining of method invocations
   */
  public GraphemeComposer add(CharSequence chars, StringBuilder builder) {
    for (int i = 0; i < chars.length(); i++) {
      add(chars.charAt(i), builder);
    }
    return this;
  }

  /**
   * Adds a character to the currently pending grapheme cluster if possible.
   *
   * <p>If the character can be added to the currently pending grapheme cluster, it gets added.
   * Otherwise the currently pending grapheme cluster is first flushed to {@code builder}. Then, if
   * the character can start a grapheme cluster, it is added to the now empty (new) grapheme
   * cluster.
   *
   * @param c the character (Unicode code point) to be added
   * @param builder the {@code StringBuilder} to which any completed clusters get appended
   * @return {@code this} for easy chaining of method invocations
   */
  public GraphemeComposer add(int c, StringBuilder builder) {
    if (!GraphemeCluster.canAdd(c)) {
      flushTo(builder);
      builder.appendCodePoint(c);
      return this;
    }
    if (isStartOfCluster(c) && graphemeCluster.isComplete()) {
      flushTo(builder);
    }
    graphemeCluster.add((char) c);
    sequence.add((char) c);
    return this;
  }

  /**
   * Removes the most recently {@code add}ed character from this sequence.
   *
   * @return {@code this} for easy chaining of method invocations
   */
  public GraphemeComposer remove() {
    sequence.remove(sequence.size() - 1);
    graphemeCluster.clear();
    for (char c : sequence) {
      graphemeCluster.add(c);
    }
    return this;
  }

  /** Returns the currently pending grapheme cluster as a {@code String}. */
  @Override
  public String toString() {
    return graphemeCluster.toString();
  }
}
