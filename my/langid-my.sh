#! /bin/bash
#
# Copyright 2016 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Identification of Burmese text.
#
# This script can be used to distinguish Burmese text (in either the Zawgyi or
# the Unicode 5.1+ coded character set) from text in other languages that use
# the same Myanmar script.
#
# A variety of languages can be written in Myanmar script. It is easy to find
# documents on the Web in Shan, Mon, S'gaw Karen, and Pali written in Myanmar
# script. Texts in other languages, such as Kachin, are somewhat harder to
# find. There further exist non-standard coded character sets that share
# codepoints with the Myanmar block of the Unicode coded character set. In light
# of this overall situation, it is often useful to distinguish Burmese text in
# known character sets from other text.
#
# Obtaining Burmese documents for training or validating a statistical model is
# straightforward. Finding documents that are not in Burmese but share
# codepoints and/or characters with Burmese is much less straightforward, in
# particular because "everything else" is an open-ended class and the full
# variability of non-Burmese text is not known a priori.
#
# The machine-learning task for this is known as One-Class Classification and is
# often approached in terms of support-vector classification. This raises the
# immediate question of what features should be used for classification. After
# much experimentation we have reduced the number of features to one, which also
# suggests a much simpler approach.
#
# The feature/statistic, call it B, which we use to distinguish Burmese text
# from everything else is the relative frequency of unseen codepoint bigrams,
# i.e. codepoint bigrams that are not found in a dictionary of the most
# frequent bigrams in Burmese text. We propose two decision rules:
#
#   1. Decide "non-Burmese" iff B > 1.5%. This is very fast to compute and
#      works well on sufficiently long documents (>100 codepoints).
#
#   2. Decide "non-Burmese" iff the p-value of a beta-binomial test for B is
#      less than a chosen significance level (i.e. reject the null hypothesis
#      of "Burmese"). Computing the p-value takes more time and is more
#      conservative on short documents. The parameters alpha and beta of the
#      beta-binomial model given below were fitted on a held-out dataset of
#      Burmese text from multiple sources.
#
# This script reads file paths from stdin (one per line), extracts
# Myanmar-script text for each file, computes the test statistic and p-value,
# and writes the results to stdout.
#
# Results are written in TSV format with the following columns:
#
#   1. path name of the file
#   2. number of unknown Myanmar codepoint bigrams in the file
#   3. total number of Myanmar codepoint bigrams in the file
#   4. percentage B of unknown Myanmar codepoint bigrams in the file
#   5. p-value under a beta-binomial null hypothesis
#   6. significance code (0 '***' 0.001 '**' 0.01 '*' 0.05 '.' 0.1 '' 1)
#
# To implement the first decision rule, check if $4 > 1.5; to implement the
# second decision rule, check if $5 < 0.0001 or some other significance level.
#
# Example invocation:
#
# $ find ../third_party/wikimedia -name 'wp_*.txt' |
#   ./langid-my.sh |
#   awk -F'\t' '$3 > 0 && ($4 > 1.5 || $5 < 0.0001)'

set -o errexit
set -o nounset
set -o pipefail

projdir="${0}.runfiles/language_resources"

if [ -d "$projdir" ]; then
  mydir="$projdir/my"
else
  mydir="$(dirname "$0")"
fi

exec "$mydir/bigrams.py" \
  "$mydir/frequent_bigrams_my.txt" 2.1633 778.8
