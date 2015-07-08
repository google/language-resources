#! /usr/bin/env python2

# Copyright 2015 Google Inc. All Rights Reserved.
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


import codecs
import re
import sys
import unicodedata

_STDIN  = codecs.lookup('utf-8').streamreader(sys.stdin)
_STDOUT = codecs.lookup('utf-8').streamwriter(sys.stdout)
_STDERR = codecs.lookup('utf-8').streamwriter(sys.stderr)

DATA_RE = re.compile(r'\( ben_([0-9]{4,4}) " ([^ ].*[^ ]) *"\)\s*$')

def ReadData(path):
  with codecs.open(path, 'r', 'utf-8') as reader:
    for line in reader:
      match = DATA_RE.match(line)
      assert match is not None
      yield match.group(1), match.group(2)
  return


## TODO: Redo the following before open sourcing

PROJECT_DIR = ('/google/src/files/head/depot/google3/' +
               'third_party/speech/tts/iiit/bengali/')

if __name__ == '__main__':
  orth = [x for x in ReadData(PROJECT_DIR + 'etc/txt.done.data.utf8')]
  pron = [x for x in ReadData(PROJECT_DIR + 'etc/txt.done.data')]
  assert len(orth) == len(pron)
  for (utt, word), (utt2, transcription) in zip(orth, pron):
    assert utt == utt2
    assert unicodedata.normalize('NFC', word) == word
    _STDOUT.write('%s\t%s\t%s\n' % (utt, word, transcription))
