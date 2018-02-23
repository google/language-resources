#! /bin/bash
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
#
# Copyright 2018 Google, Inc.

LANG_PATH="/usr/local/src/language-resources/vb/festvox/"
mkdir -p "${LANG_PATH}"

# Copy non wav resources to language-resources
cp /mnt/data/txt.done.data "${LANG_PATH}"
cp /mnt/data/ipa_phonology.json "${LANG_PATH}"
cp /mnt/data/lexicon.scm "${LANG_PATH}"

# Untar the wavs
mkdir /mnt/data/wavs && cd /mnt/data/wavs
if [[ -f ../wav.tar ]]; then
  tar -xf ../wav.tar
fi
if [[ -f ../wav.tar.gz ]]; then
  tar -xvzf ../wav.tar.gz
fi
cd ../

# Make voice training dir
mkdir /mnt/data/voice

# Start festival training
cd /usr/local/src/language-resources
/usr/local/src/language-resources/festival_utils/build_festvox_voice.sh \
/mnt/data/wavs \
vb \
/mnt/data/voice

# Package built voice
cd /mnt/data/voice
tar --absolute-names -zcvf /mnt/data/build_voice.tar.gz .
ls -l /mnt/data/
