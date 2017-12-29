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

cd /

# 1. Test TTS resources.
find /usr/local/src/voice/data/ > /tmp/actual_tts_data_files_list.txt
DIFF=$(diff /usr/local/src/tts_data_files_list.txt /tmp/actual_tts_data_files_list.txt | wc -l)

if [[ $DIFF != 0 ]]; then
  echo "[Test] TTS resources failed"
fi

# 2. Test merlin conf files.
find /usr/local/src/merlin/egs/locale/s1/conf/ > /tmp/actual_merlin_conf_file_list.txt
DIFF=$(diff /usr/local/src/merlin_conf_file_list.txt /tmp/actual_merlin_conf_file_list.txt | wc -l)

if [[ $DIFF != 0 ]]; then
  echo "[Test] Merlin conf files failed"
fi

# 3. Test merlin data files.
find /usr/local/src/merlin/egs/locale/s1/data/ > /tmp/actual_merlin_data_file_list.txt
DIFF=$(diff /usr/local/src/merlin_data_file_list.txt /tmp/actual_merlin_data_file_list.txt | wc -l)

if [[ $DIFF != 0 ]]; then
  echo "[Test] Merlin data files failed"
fi

# 4. Test merlin acoustic model files.
find /usr/local/src/merlin/egs/locale/s1/models/acoustic_model/gen/ > /tmp/actual_merlin_models_acoustic_gen_file_list.txt
DIFF=$(diff /usr/local/src/merlin_models_acoustic_gen_file_list.txt /tmp/actual_merlin_models_acoustic_gen_file_list.txt | wc -l)

if [[ $DIFF != 0 ]]; then
  echo "[Test] Merlin acoustic model files failed"
fi

echo "Testing completed"
