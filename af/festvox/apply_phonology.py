#! /usr/bin/python2
# -*- coding: utf-8 -*-
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

"""Tweaks a directory tree set up for a Festvox Clustergen build.

Reads a phonology description and writes/updates several files in a
Festvox Clustergen build tree, including the phonset definition and
various feature and feature description files.

"""

from __future__ import unicode_literals

import codecs
import glob
import json
import os.path
import re
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)

INST_LANG_VOX = re.compile(r'.*/([^_]+_[^_]+)_([^_]+)_phoneset.scm$')


def ReadPhonology(path):
  with codecs.open(path, 'r', 'utf-8') as reader:
    contents = reader.read()
  return json.loads(contents)


def MakePhonesetScm(writer, phonology, inst_lang, vox):
  writer.write(';;; Automatically generated. Edit with caution.\n\n')
  writer.write('(defPhoneSet\n  %s\n  (\n' % phonology['name'])
  # A good amount of the following logic is specific to how we chose to handle
  # Afrikaans. TODO: Generalize.
  assert len(phonology['features']) == 11
  for feature in phonology['features']:
    writer.write('   (%s 0)\n' % ' '.join(feature))
  writer.write('   )\n  (\n')
  silences = []
  for phone in phonology['phones']:
    assert len(phone) >= 2
    if phone[1] == 'consonant':
      assert len(phone) == 5
      writer.write('   (%-3s %s %-9s %-13s %s 0 0 0 0 0 0 0)\n' %
                   tuple(phone))
    elif phone[1] == 'vowel':
      assert len(phone) == 6
      writer.write('   (%-3s %s 0 0 0 %-9s %-8s %-7s %-9s %-4s %-7s %s)\n' %
                   tuple(phone[:-1] + [0, 0, 0, phone[-1]]))
    elif phone[1] == 'diph':
      assert len(phone) == 9
      writer.write('   (%-3s %-5s 0 0 0 %-9s %-8s %-7s %-9s %-4s %-7s %s)\n' %
                   tuple(phone))
    else:
      assert phone[1] == 'silence'
      assert len(phone) == 2
      writer.write('   (%-3s %s 0 0 0 0 0 0 0 0 0 0)\n' % tuple(phone))
      silences.append(phone[0])
  writer.write('  )\n)\n\n')
  writer.write("(PhoneSet.silences '(%s))\n\n" % ' '.join(silences))
  writer.write(r'''(define (%s_%s::select_phoneset)
  "(%s_%s::select_phoneset)
Set up phone set for %s."
  (Parameter.set 'PhoneSet '%s)
  (PhoneSet.select '%s)
)

(define (%s_%s::reset_phoneset)
  "(%s_%s::reset_phoneset)
Reset phone set for %s."
  t
)

(provide '%s_%s_phoneset)
''' % (inst_lang, vox,
       inst_lang, vox,
       inst_lang,
       phonology['name'],
       phonology['name'],
       inst_lang, vox,
       inst_lang, vox,
       inst_lang,
       inst_lang, vox)
  )
  return


def MakeLexiconScm(writer, inst_lang, vox):
  writer.write(r''';;; Automatically generated. Edit with caution.

(lex.create "%s")
(lex.set.phoneset "%s")
(lex.set.compile.file "festvox/lexicon.scm")
(lex.select "%s")

(define (%s_%s::select_lexicon)
  "(%s_%s::select_lexicon)
Set up the lexicon for %s."
  (lex.select "%s")
)
(define (%s_%s::reset_lexicon)
  "(%s_%s::reset_lexicon)
Reset lexicon information."
  t
)
(provide '%s_%s_lexicon)
''' % (inst_lang,
       inst_lang,
       inst_lang,
       inst_lang, vox,
       inst_lang, vox,
       inst_lang,
       inst_lang,
       inst_lang, vox,
       inst_lang, vox,
       inst_lang, vox)
  )
  return


def MakeAllDesc(writer, phones, features):
  writer.write('(\n( occurid cluster )\n')

  writer.write('( p.name 0 ')
  writer.write(' '.join('"%s"' % p for p in phones))
  writer.write(')\n')

  for k, vs in features:
    writer.write('( p.ph_%s %s )\n' % (k, vs))

  writer.write('( n.name ignore 0 ')
  writer.write(' '.join('"%s"' % p for p in phones))
  writer.write(')\n')

  for k, vs in features:
    writer.write('( n.ph_%s %s )\n' % (k, vs))

  writer.write(r'''( segment_duration ignore float)
( seg_pitch ignore float )
( p.seg_pitch ignore float )
( n.seg_pitch ignore float )
( R:SylStructure.parent.stress
0
1
)
( seg_onsetcoda
coda
onset
)
( n.seg_onsetcoda
0
coda
onset
)
( p.seg_onsetcoda
0
coda
onset
)
( R:SylStructure.parent.accented
0
1
)
( pos_in_syl
0
1
2
3
4
5
6
7
8
)
( syl_initial
0
1
)
( syl_final
0
1
)
( R:SylStructure.parent.lisp_cg_break
0
1
2
3
4
)
( R:SylStructure.parent.R:Syllable.p.lisp_cg_break
0
1
2
3
4
)
( R:SylStructure.parent.position_type
0
initial
single
final
mid
)
''')

  writer.write('( pp.name ignore 0 ')
  writer.write(' '.join('"%s"' % p for p in phones))
  writer.write(')\n')

  for k, vs in features:
    writer.write('( pp.ph_%s ignore %s )\n' % (k, vs))

  writer.write(r'''( n.lisp_is_pau 0 1)
( p.lisp_is_pau 0 1)
( R:SylStructure.parent.parent.gpos
0
aux
cc
content
det
in
md
pps
to
wp
punc
)
( R:SylStructure.parent.parent.R:Word.p.gpos
0
aux
cc
content
det
in
md
pps
to
wp
punc
)
( R:SylStructure.parent.parent.R:Word.n.gpos
0
aux
cc
content
det
in
md
pps
to
wp
punc
)
)
''')
  return


def MakeMcepDesc(writer, phones, features):
  writer.write(r'''(
( occurid vector )
( R:mcep_link.parent.name
state names
"0"
)
( R:mcep_link.parent.R:HMMstate.p.name
state names
"0"
)
( R:mcep_link.parent.R:HMMstate.n.name
state names
"0"
)
( R:mcep_link.parent.R:segstate.parent.p.name
phone names
"0"
)
''')

  for k, vs in features:
    writer.write('( R:mcep_link.parent.R:segstate.parent.p.ph_%s %s )\n'
                 % (k, vs))

  writer.write(r'''( R:mcep_link.parent.R:segstate.parent.n.name ignore
phone names
"0"
)
''')

  for k, vs in features:
    writer.write('( R:mcep_link.parent.R:segstate.parent.n.ph_%s %s )\n'
                 % (k, vs))

  writer.write(r'''( R:mcep_link.parent.R:segstate.parent.segment_duration ignore float)
( R:mcep_link.parent.lisp_cg_duration float)
( R:mcep_link.parent.R:segstate.n.lisp_cg_duration float)
( R:mcep_link.parent.R:segstate.p.lisp_cg_duration float)
( lisp_cg_state_pos b m e)
( lisp_cg_state_place float)
( lisp_cg_state_index float)
( lisp_cg_state_rindex float)
( lisp_cg_phone_place float)
( lisp_cg_phone_index float)
( lisp_cg_phone_rindex float)
( lisp_cg_position_in_sentence ignore float)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.parent.R:Phrase.parent.lisp_cg_find_phrase_number float)
( lisp_cg_position_in_phrasep float)
( lisp_cg_position_in_phrase float)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.stress float)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.R:Syllable.p.stress
float
)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.R:Syllable.n.stress
float
)
( R:mcep_link.parent.R:segstate.parent.seg_onsetcoda
0
coda
onset
)
( R:mcep_link.parent.R:segstate.parent.n.seg_onsetcoda
0
coda
onset
)
( R:mcep_link.parent.R:segstate.parent.p.seg_onsetcoda
0
coda
onset
)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.accented
0
1
)
( R:mcep_link.parent.R:segstate.parent.pos_in_syl float)
( R:mcep_link.parent.R:segstate.parent.syl_initial
0
1
)
( R:mcep_link.parent.R:segstate.parent.syl_final
0
1
)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.lisp_cg_break
0
1
3
4
)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.R:Syllable.p.lisp_cg_break
0
1
3
4
)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.position_type
0
initial
single
final
mid
)
( R:mcep_link.parent.R:segstate.parent.pp.name ignore
phone names
"0"
)
''')

  for k, vs in features:
    writer.write('( R:mcep_link.parent.R:segstate.parent.pp.ph_%s ignore %s )\n'
                 % (k, vs))

  writer.write(r'''( R:mcep_link.parent.R:segstate.parent.n.lisp_is_pau 0 1)
( R:mcep_link.parent.R:segstate.parent.p.lisp_is_pau 0 1)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.parent.gpos
0
aux
cc
content
det
in
md
pps
to
wp
punc
)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.parent.R:Word.p.gpos
0
aux
cc
content
det
in
md
pps
to
wp
punc
)
( R:mcep_link.parent.R:segstate.parent.R:SylStructure.parent.parent.R:Word.n.gpos
0
aux
cc
content
det
in
md
pps
to
wp
punc
)
)
''')
  return


def MakeMceptrajDesc(writer, phones, features):
  writer.write(r'''(
( occurid trajectory )
( num_frames ignore int)
( name
state names
"0"
)
( R:HMMstate.p.name
state names
"0"
)
( R:HMMstate.n.name
state names
"0"
)
( R:segstate.parent.p.name
phone names
"0"
)
''')

  for k, vs in features:
    writer.write('( R:segstate.parent.p.ph_%s %s )\n'
                 % (k, vs))

  writer.write(r'''( R:segstate.parent.n.name ignore
phone names
"0"
)
''')

  for k, vs in features:
    writer.write('( R:segstate.parent.n.ph_%s %s )\n'
                 % (k, vs))

  writer.write(r'''( R:segstate.parent.segment_duration ignore float)
( lisp_cg_duration float)
( n.lisp_cg_duration float)
( p.lisp_cg_duration float)
( lisp_cg_state_pos b m e)
( n.lisp_cg_state_pos 0 b m e)
( p.lisp_cg_state_pos 0 b m e)
( p.p.lisp_cg_state_pos 0 b m e)
( n.n.lisp_cg_state_pos 0 b m e)
( R:segstate.parent.R:SylStructure.parent.stress
0
1
)
( R:segstate.parent.seg_onsetcoda
coda
onset
)
( R:segstate.parent.n.seg_onsetcoda
0
coda
onset
)
( R:segstate.parent.p.seg_onsetcoda
0
coda
onset
)
( R:segstate.parent.R:SylStructure.parent.accented
0
1
)
( R:segstate.parent.pos_in_syl float )
( R:segstate.parent.syl_initial
0
1
)
( R:segstate.parent.syl_final
0
1
)
( R:segstate.parent.R:SylStructure.parent.syl_break
0
1
2
3
4
)
( R:segstate.parent.R:SylStructure.parent.R:Syllable.p.syl_break
0
1
2
3
4
)
( R:segstate.parent.R:SylStructure.parent.position_type
0
initial
single
final
mid
)
( R:segstate.parent.pp.name ignore
phone names
"0"
)
''')

  for k, vs in features:
    writer.write('( R:segstate.parent.pp.ph_%s ignore %s )\n'
                 % (k, vs))

  writer.write(r'''( R:segstate.parent.n.lisp_is_pau 0 1)
( R:segstate.parent.p.lisp_is_pau 0 1)
( R:segstate.parent.R:SylStructure.parent.parent.gpos
0
aux
cc
content
det
in
md
pps
to
wp
)
( R:segstate.parent.R:SylStructure.parent.parent.R:Word.p.gpos
0
aux
cc
content
det
in
md
pps
to
wp
)
( R:segstate.parent.R:SylStructure.parent.parent.R:Word.n.gpos
0
aux
cc
content
det
in
md
pps
to
wp
)
)
''')
  return


def MakeDurFeats(writer, phones, features):
  writer.write(r'''
lisp_zscore_dur
name
p.name
n.name
R:SylStructure.parent.syl_onsetsize
R:SylStructure.parent.syl_codasize
R:SylStructure.parent.R:Syllable.n.syl_onsetsize
R:SylStructure.parent.R:Syllable.p.syl_codasize
R:SylStructure.parent.position_type
R:SylStructure.parent.parent.word_numsyls
pos_in_syl
syl_initial
syl_final
R:SylStructure.parent.pos_in_word
p.seg_onsetcoda
seg_onsetcoda
n.seg_onsetcoda
''')

  for k, _ in features:
    for prefix in ('pp.', 'p.', '', 'n.', 'nn.'):
      writer.write('%sph_%s\n' % (prefix, k))

  writer.write(r'''R:SylStructure.parent.R:Syllable.pp.accented
R:SylStructure.parent.R:Syllable.p.accented
R:SylStructure.parent.accented
R:SylStructure.parent.R:Syllable.n.accented
R:SylStructure.parent.R:Syllable.nn.accented
R:SylStructure.parent.R:Syllable.pp.syl_break
R:SylStructure.parent.R:Syllable.p.syl_break
R:SylStructure.parent.syl_break
R:SylStructure.parent.R:Syllable.n.syl_break
R:SylStructure.parent.R:Syllable.nn.syl_break
R:SylStructure.parent.R:Syllable.pp.stress
R:SylStructure.parent.R:Syllable.p.stress
R:SylStructure.parent.stress
R:SylStructure.parent.R:Syllable.n.stress
R:SylStructure.parent.R:Syllable.nn.stress
R:SylStructure.parent.syl_in
R:SylStructure.parent.syl_out
R:SylStructure.parent.ssyl_in
R:SylStructure.parent.ssyl_out
R:SylStructure.parent.asyl_in
R:SylStructure.parent.asyl_out
R:SylStructure.parent.last_accent
R:SylStructure.parent.next_accent
R:SylStructure.parent.sub_phrases
lisp_onset_stop
lisp_onset_fric
lisp_onset_nasal
lisp_onset_glide
lisp_coda_stop
lisp_coda_fric
lisp_coda_nasal
lisp_coda_glide
R:SylStructure.parent.parent.pos
R:SylStructure.parent.parent.gpos
R:SylStructure.parent.parent.phr_pos
''')
  return


def MakeStatedurFeats(writer, phones, features):
  writer.write(r'''lisp_zscore_dur
name
p.name
n.name
R:segstate.parent.name
R:segstate.parent.p.name
R:segstate.parent.n.name
R:segstate.parent.R:SylStructure.parent.syl_onsetsize
R:segstate.parent.R:SylStructure.parent.syl_codasize
R:segstate.parent.R:SylStructure.parent.R:Syllable.n.syl_onsetsize
R:segstate.parent.R:SylStructure.parent.R:Syllable.p.syl_codasize
R:segstate.parent.R:SylStructure.parent.position_type
R:segstate.parent.R:SylStructure.parent.parent.word_numsyls
R:segstate.parent.pos_in_syl
R:segstate.parent.syl_initial
R:segstate.parent.syl_final
R:segstate.parent.R:SylStructure.parent.pos_in_word
R:segstate.parent.p.seg_onsetcoda
R:segstate.parent.seg_onsetcoda
R:segstate.parent.n.seg_onsetcoda
''')

  for k, _ in features:
    for prefix in ('pp.', 'p.', '', 'n.', 'nn.'):
      writer.write('R:segstate.parent.%sph_%s\n' % (prefix, k))

  writer.write(r'''R:segstate.parent.R:SylStructure.parent.R:Syllable.pp.accented
R:segstate.parent.R:SylStructure.parent.R:Syllable.p.accented
R:segstate.parent.R:SylStructure.parent.accented
R:segstate.parent.R:SylStructure.parent.R:Syllable.n.accented
R:segstate.parent.R:SylStructure.parent.R:Syllable.nn.accented
R:segstate.parent.R:SylStructure.parent.R:Syllable.pp.lisp_cg_break
R:segstate.parent.R:SylStructure.parent.R:Syllable.p.lisp_cg_break
R:segstate.parent.R:SylStructure.parent.lisp_cg_break
R:segstate.parent.R:SylStructure.parent.R:Syllable.n.lisp_cg_break
R:segstate.parent.R:SylStructure.parent.R:Syllable.nn.lisp_cg_break
R:segstate.parent.R:SylStructure.parent.R:Syllable.pp.stress
R:segstate.parent.R:SylStructure.parent.R:Syllable.p.stress
R:segstate.parent.R:SylStructure.parent.stress
R:segstate.parent.R:SylStructure.parent.R:Syllable.n.stress
R:segstate.parent.R:SylStructure.parent.R:Syllable.nn.stress
R:segstate.parent.R:SylStructure.parent.syl_in
R:segstate.parent.R:SylStructure.parent.syl_out
R:segstate.parent.R:SylStructure.parent.ssyl_in
R:segstate.parent.R:SylStructure.parent.ssyl_out
R:segstate.parent.R:SylStructure.parent.asyl_in
R:segstate.parent.R:SylStructure.parent.asyl_out
R:segstate.parent.R:SylStructure.parent.last_accent
R:segstate.parent.R:SylStructure.parent.next_accent
R:segstate.parent.R:SylStructure.parent.sub_phrases
R:segstate.parent.lisp_onset_stop
R:segstate.parent.lisp_onset_fric
R:segstate.parent.lisp_onset_nasal
R:segstate.parent.lisp_onset_glide
R:segstate.parent.lisp_coda_stop
R:segstate.parent.lisp_coda_fric
R:segstate.parent.lisp_coda_nasal
R:segstate.parent.lisp_coda_glide
R:segstate.parent.R:SylStructure.parent.parent.pos
R:segstate.parent.R:SylStructure.parent.parent.gpos
R:segstate.parent.R:SylStructure.parent.parent.phr_pos
''')
  return


def main(argv):
  if len(argv) != 3:
    sys.stdout.write('Usage: %s phonology.json path/to/tts/build/dir\n' %
                     argv[0])
    sys.exit(2)

  phonology = ReadPhonology(argv[1])
  build_dir = os.path.normpath(argv[2])

  phoneset_paths = glob.glob('%s/festvox/*_phoneset.scm' % build_dir)
  assert len(phoneset_paths) == 1
  phoneset_path = phoneset_paths[0]
  match = INST_LANG_VOX.match(phoneset_path)
  assert match
  inst_lang = match.group(1)
  vox = match.group(2)

  with codecs.open(phoneset_path, 'w', 'utf-8') as writer:
    MakePhonesetScm(writer, phonology, inst_lang, vox)

  with codecs.open('%s/festvox/%s_%s_lexicon.scm' % (build_dir, inst_lang, vox),
                   'w', 'utf-8') as writer:
    MakeLexiconScm(writer, inst_lang, vox)

  phones = [phone[0] for phone in phonology['phones']]
  features = [(feature[0], ' '.join(feature[1:] + ['0']))
              for feature in phonology['features']]
  STDERR.write('phones: %s\n' % ' '.join(phones))
  for k, vs in features:
    STDERR.write('feature %s: %s\n' % (k, vs))

  fpath = '%s/festival' % build_dir
  with codecs.open('%s/clunits/all.desc' % fpath, 'w', 'utf-8') as writer:
    MakeAllDesc(writer, phones, features)
  with codecs.open('%s/clunits/mcep.desc' % fpath, 'w', 'utf-8') as writer:
    MakeMcepDesc(writer, phones, features)
  with codecs.open('%s/clunits/mceptraj.desc' % fpath, 'w', 'utf-8') as writer:
    MakeMceptrajDesc(writer, phones, features)
  with codecs.open('%s/dur/etc/dur.feats' % fpath, 'w', 'utf-8') as writer:
    MakeDurFeats(writer, phones, features)
  with codecs.open('%s/dur/etc/statedur.feats' % fpath, 'w', 'utf-8') as writer:
    MakeStatedurFeats(writer, phones, features)
  return


if __name__ == '__main__':
  main(sys.argv)
