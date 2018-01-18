#!/bin/bash
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
#
# Simple script to run merlin training in docker.
#

set -x 
set -e

PY_MERLIN_CONF="/usr/local/src/language-resources/festival_utils/merlin_confs.py"
BASE_VOICE_PATH="/usr/local/src/merlin//egs/locale/s1"
CONF_PATH="egs/locale/s1/conf/"

# Whether to run merlin TTS training.
TRAIN=true
SAMPLE_RATE=48000
MERLIN_GLOBAL_CONFIG=/usr/local/src/language-resources/docker-images/merlin/small_merlin_params.json

while getopts ":t" opt; do
  case ${opt} in
    t ) # Option to disable training.
      TRAIN=false
      echo "Disable merlin training"
      ;;
    r) # Set sample rate.
      SAMPLE_RATE="${OPTARG}"
      ;;
    c) # Path to the merlin global json config.
      MERLIN_GLOBAL_CONFIG="${OPTARG}"
      ;;
  esac
done

# Setup merlin files.
cd /usr/local/src/voice
/usr/local/src/language-resources/festival_utils/setup_merlin.sh \
/usr/local/src/voice/training \
/usr/local/src/merlin/ \
/usr/local/src/voice/data/festvox/wavs/ \
data \
locale \
"${SAMPLE_RATE}"

# Setup required paths.
mkdir -p ${BASE_VOICE_PATH}/models/duration_model
mkdir -p ${BASE_VOICE_PATH}/models/acoustic_model
mkdir -p ${BASE_VOICE_PATH}/models/test_synthesis

# Fix merlin conf files.
cd /usr/local/src/merlin
COUNT=$(wc -l < ${BASE_VOICE_PATH}/data/file_id_list.scp)

for conf in acoustic_dnn,acoustic duration_dnn,duration; do
  IFS=',' read conf conf_type <<< "${i}"
  S=${CONF_PATH}/_${conf}.conf
  D=${CONF_PATH}/${conf}.conf
  mv  ${D} ${S}

  # Usage - python merlin_confs.py data_count conf_file global_config {acoustic,duration}
  python ${PY_MERLIN_CONF} ${S} "${COUNT}" ${MERLIN_GLOBAL_CONFIG} ${conf_type} > ${D}
  rm ${S}
done

# Run merlin training.
if [[ ${TRAIN} == true ]]; then
  echo "Training merlin ${LANG} voice"
  time python src/run_merlin.py egs/locale/s1/conf/duration_dnn.conf
  time python src/run_merlin.py egs/locale/s1/conf/acoustic_dnn.conf
fi
