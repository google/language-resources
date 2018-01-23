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
# Copyright 2017 Google, Inc.

set -x
set -e

# Remove inbuilt langauge resources and download new one.
cd /usr/local/src/
rm -rf language-resources
git clone https://github.com/googlei18n/language-resources

# Untar the wav files.
mkdir -p /usr/local/src/voice/data/festvox/wavs
cd /usr/local/src/voice/data/festvox/wavs
if [[ -f ../wav.tar ]]; then
  tar -xf ../wav.tar
fi
if [[ -f ../wav.tar.gz ]]; then
  tar -xvzf ../wav.tar.gz
fi

# Debug information.
cd /usr/local/src/voice/data/festvox/
echo "List of data"
find .
echo "$(pwd)"

# Setup festival
cd /usr/local/src/voice
mkdir training
/usr/local/src/language-resources/festival_utils/build_festvox_voice.sh -t \
/usr/local/src/voice/data/festvox/wavs/ \
data \
/usr/local/src/voice/training

# Generate utts, initial labels and alignments
cd /usr/local/src/voice/training
./bin/do_build parallel build_prompts
./bin/do_build label
./bin/do_clustergen parallel build_utts
cd lab && ls *.lab -1 | sed -e 's/\.lab$//' | head -n 30 > ../file_id_list.scp

# Setup merlin
cd /usr/local/src/voice/training
/usr/local/src/language-resources/festival_utils/run_docker_merlin_training.sh

# Package the voice
cd /usr/local/src/voice/data/festvox
tar --absolute-names -zcvf built-voice.tar.gz /usr/local/src/merlin/egs/locale /usr/local/src/voice/training
