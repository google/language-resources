#! /usr/bin/env python
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
r"""Generate HTS question file from json phonology.

Questions are generated according to the [1] format with support for Merlin CQS
tags.

[1] -
https://wiki.inf.ed.ac.uk/twiki/pub/CSTR/F0parametrisation/hts_lab_format.pdf

Usage
  ./festival_utils/generate_hts_questions.py si/festvox/ipa_phonology.json
"""

__author__ = "pasindu@google.com (Pasindu De Silva)"

import io
import json
import sys

STDOUT = io.open(1, mode="wt", encoding="utf-8", closefd=False)

STATIC_QUESTIONS = r"""QS "C-Syl_Vowel==x"      {|x/C:}
QS "C-Syl_Vowel==no"    {|novowel/C:}
QS "L-Word_GPOS==0"     {/D:0_}
QS "L-Word_GPOS==aux"   {/D:aux_}
QS "L-Word_GPOS==cc"    {/D:cc_}
QS "L-Word_GPOS==content" {/D:content_}
QS "L-Word_GPOS==det"   {/D:det_}
QS "L-Word_GPOS==in"    {/D:in_}
QS "L-Word_GPOS==md"    {/D:md_}
QS "L-Word_GPOS==pps"   {/D:pps_}
QS "L-Word_GPOS==punc"    {/D:punc_}
QS "L-Word_GPOS==to"    {/D:to_}
QS "L-Word_GPOS==wp"    {/D:wp_}
QS "C-Word_GPOS==x"   {/E:x+}
QS "C-Word_GPOS==aux"   {/E:aux+}
QS "C-Word_GPOS==cc"    {/E:cc+}
QS "C-Word_GPOS==content" {/E:content+}
QS "C-Word_GPOS==det"   {/E:det+}
QS "C-Word_GPOS==in"    {/E:in+}
QS "C-Word_GPOS==md"    {/E:md+}
QS "C-Word_GPOS==pps"   {/E:pps+}
QS "C-Word_GPOS==punc"    {/E:punc+}
QS "C-Word_GPOS==to"    {/E:to+}
QS "C-Word_GPOS==wp"    {/E:wp+}
QS "R-Word_GPOS==0"     {/F:0_}
QS "R-Word_GPOS==aux"   {/F:aux_}
QS "R-Word_GPOS==cc"    {/F:cc_}
QS "R-Word_GPOS==content" {/F:content_}
QS "R-Word_GPOS==det"   {/F:det_}
QS "R-Word_GPOS==in"    {/F:in_}
QS "R-Word_GPOS==md"    {/F:md_}
QS "R-Word_GPOS==pps"   {/F:pps_}
QS "R-Word_GPOS==punc"    {/F:punc_}
QS "R-Word_GPOS==to"    {/F:to_}
QS "R-Word_GPOS==wp"    {/F:wp_}
CQS "Seg_Fw"                                      {@(\d+)_}
CQS "Seg_Bw"                                      {_(\d+)/A:}
CQS "L-Syl_Stress"                                {/A:(\d+)_}
CQS "L-Syl_Accent"                                {_(\d+)_}
CQS "L-Syl_Num-Segs"                              {_(\d+)/B:}
CQS "C-Syl_Stress"                                {/B:(\d+)-}
CQS "C-Syl_Accent"                                {-(\d+)-}
CQS "C-Syl_Num-Segs"                              {-(\d+)@}
CQS "Pos_C-Syl_in_C-Word(Fw)"                     {@(\d+)-}
CQS "Pos_C-Syl_in_C-Word(Bw)"                     {-(\d+)&}
CQS "Pos_C-Syl_in_C-Phrase(Fw)"                     {&(\d+)-}
CQS "Pos_C-Syl_in_C-Phrase(Bw)"                     {-(\d+)#}
CQS "Num-StressedSyl_before_C-Syl_in_C-Phrase"      {#(\d+)-}
CQS "Num-StressedSyl_after_C-Syl_in_C-Phrase"   {-(\d+)$}
CQS "Num-AccentedSyl_before_C-Syl_in_C-Phrase"      {$(\d+)-}
CQS "Num-AccentedSyl_after_C-Syl_in_C-Phrase"     {-(\d+)!}
CQS "Num-Syl_from_prev-StressedSyl"               {!(\d+)-}
CQS "Num-Syl_from_next-StressedSyl"                 {-(\d+);}
CQS "Num-Syl_from_prev-AccentedSyl"                 {;(\d+)-}
CQS "Num-Syl_from_next-AccentedSyl"                 {-(\d+)|}
CQS "R-Syl_Stress"                                {/C:(\d+)+}
CQS "R-Syl_Accent"                                {+(\d+)+}
CQS "R-Syl_Num-Segs"                              {+(\d+)/D:}
CQS "L-Word_Num-Syls"                             {_(\d+)/E:}
CQS "C-Word_Num-Syls"                             {+(\d+)@}
CQS "Pos_C-Word_in_C-Phrase(Fw)"                  {@(\d+)+}
CQS "Pos_C-Word_in_C-Phrase(Bw)"                  {+(\d+)&}
CQS "Num-ContWord_before_C-Word_in_C-Phrase"      {&(\d+)+}
CQS "Num-ContWord_after_C-Word_in_C-Phrase"         {+(\d+)#}
CQS "Num-Words_from_prev-ContWord"                  {#(\d+)+}
CQS "Num-Words_from_next-ContWord"                  {+(\d+)/F:}
CQS "R-Word_Num-Syls"                             {_(\d+)/G:}
CQS "L-Phrase_Num-Syls"                             {/G:(\d+)_}
CQS "L-Phrase_Num-Words"                          {_(\d+)/H:}
CQS "C-Phrase_Num-Syls"                             {/H:(\d+)=}
CQS "C-Phrase_Num-Words"                          {=(\d+)@}
CQS "Pos_C-Phrase_in_Utterance(Fw)"                 {@(\d+)=}
CQS "Pos_C-Phrase_in_Utterance(Bw)"                 {=(\d+)&}
CQS "R-Phrase_Num-Syls"                             {/I:(\d+)=}
CQS "R-Phrase_Num-Words"                          {=(\d+)/J:}
CQS "Num-Syls_in_Utterance"                         {/J:(\d+)+}
CQS "Num-Words_in_Utterance"                      {+(\d+)-}
CQS "Num-Phrases_in_Utterance"                    {-(\d+)}
"""

PHONEME_QUESTIONS = [["C", "-%s+"], ["L", "^%s-"], ["LL", "%s^"], ["R", "+%s="],
                     ["RR", "=%s@"], ["C-Syl", "|%s/C:"]]


def main(argv):
  phonology = json.loads(open(argv[1]).read())
  content = ""

  # Add phoneme questions.
  #  - Example - QS "C-uw"  {-uw+}
  for qs in PHONEME_QUESTIONS:
    for phoneinfo in phonology["phones"]:
      phoneme = phoneinfo[0]
      content += "QS \"%s-%s\"\t\t\t\t{%s}\n" % (qs[0], phoneme,
                                                 qs[1] % (phoneme))

  # Add phoneme feature questions.
  qs_list = {}
  for phoneinfo in phonology["phones"]:
    phoneme = phoneinfo[0]
    features = phoneinfo[1:]

    for feature in features:
      if feature in qs_list:
        qs_list.get(feature).append(phoneme)
      else:
        qs_list.update({feature: [phoneme]})

  # Example phoneme feature question.
  # - QS "C-Vowel"  {-aa+,-ae+,-ah+}
  for x in qs_list:
    content += "QS \"C-" + x + "\"\t\t\t\t{%s}\n" % (
        ",".join([("-%s+") % y for y in qs_list.get(x)]))

  # Add syllabification questions.
  # Example - QS "C-Syl_aa"  {|aa/C:}
  for x in qs_list:
    content += "QS \"C-Syl-" + x + "\"\t\t\t\t{%s}\n" % (
        ",".join([("|%s/C:") % y for y in qs_list.get(x)]))

  content += STATIC_QUESTIONS

  STDOUT.write(content)


if __name__ == "__main__":
  main(sys.argv)
