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

cd /usr/local/src/voice/data/festvox

# Temp test sentence.
echo "aaron" > /tmp/text.txt

# Copied from https://github.com/googlei18n/language-resources/blob/abb8ca9746d67293d0b09bca71b2eb06261386e0/festival_utils/setup_merlin.sh#L123
TMP_MERLIN_SCRIPTS_PATH="/tmp/merlin_scripts/"
MERLIN_PATH="/usr/local/src/merlin"
FESTIVAL_VOICE_PATH="/usr/local/src/voice/training"
SYNTH_DIR="/usr/local/src/voice/training/synth"

# Clean files from the previous run.
rm -rf "${TMP_MERLIN_SCRIPTS_PATH}"
mkdir -p "${TMP_MERLIN_SCRIPTS_PATH}"
cp -R -T "${MERLIN_PATH}/misc/scripts/frontend/festival_utt_to_lab" "${TMP_MERLIN_SCRIPTS_PATH}"

# Find the name of festival phoneset file.
PHONESET_FILE_PATH=$(find "$FESTIVAL_VOICE_PATH"/festvox/*phoneset.scm)
LOAD_PHONESET_SCRIPT="(load \"$PHONESET_FILE_PATH\") "

# Load phonemeset before generating the labels.
# This adds a lisp cmd to load the current festival setup's phoneset to the 49th line
# of the extra_feats.scm (which is between the license and start of the code) . This is
# because extra_feats.scm defaults to the radio phoneset.
#
# TODO(pasindu): Make this cleaner.
sed -i  '49s|$|'"$LOAD_PHONESET_SCRIPT"'|' \
      "${TMP_MERLIN_SCRIPTS_PATH}/extra_feats.scm"

# Setup festvox for given data
cd /usr/local/src/voice
rm -rf ${SYNTH_DIR}
mkdir -p ${SYNTH_DIR}
cat /tmp/text.txt > ${SYNTH_DIR}/text.txt
echo "synth" >  ${SYNTH_DIR}/file_id.scp

cd /usr/local/src/voice/training

# Generator festival scripts to generator festival utt.
python "${MERLIN_PATH}"/misc/scripts/frontend/utils/genScmFile.py \
"${SYNTH_DIR}" \
"${SYNTH_DIR}"/ \
"${SYNTH_DIR}"/utt_generator.scm \
"${SYNTH_DIR}"/file_id.scp

# Generator festival utt.
festival -b festvox/goog_data_unison_cg.scm \
-b "(Parameter.set 'Duration_Method 'Default)"  \
-b synth/utt_generator.scm 

# Dump features.
sh ${TMP_MERLIN_SCRIPTS_PATH}/make_labels \
  "synth/" \
  "synth/" \
  "${FESTIVALDIR}/examples/dumpfeats" \
  "${TMP_MERLIN_SCRIPTS_PATH}"

# Remove leading space from the label files.
FESTIVAL_LABEL_PHONE_ALIGN=synth/full/
sed -i -r "s/^ *//g" "${FESTIVAL_LABEL_PHONE_ALIGN}"/*.lab
sed -i -r "s/  */ /g" "${FESTIVAL_LABEL_PHONE_ALIGN}"/*.lab

# Copy generated labels and test list.
cd /usr/local/src/voice/training
mkdir -p /usr/local/src/merlin/egs/locale/s1/models/test_synthesis/wav
cp "${SYNTH_DIR}/file_id.scp" /usr/local/src/merlin/egs/locale/s1/data/test_id_list.scp
cp "${SYNTH_DIR}/full/text.lab" /usr/local/src/merlin/egs/locale/s1/data/label_phone_align/ 


cd /usr/local/src/merlin

# Predict duration parameters.
python src/run_merlin.py egs/locale/s1/conf/test_dur_synth.conf

# Predict acoustics parameters and generate wav.
python src/run_merlin.py egs/locale/s1/conf/test_synth.conf

# Generated wav.
ls -l /usr/local/src/merlin/egs/locale/s1/models/test_synthesis/wav/text.wav
