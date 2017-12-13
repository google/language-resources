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
# This script creates all the resources needed for a merlin voice.
# This script requires an already existing festival setup to generate HTS labels and audio data.
#
#   - Create directories needed by merlin.
#   - Compile merlin tools.
#   - Copy wavs from Festival to Melrin.
#   - Extract audio features.
#   - Setup merlin configuration files.
#   - Generate HTS state labels (TODO).
#   - Generate HTS phone labels.
#   - Copy state and phone labels.
#   - Generate HTS questions.
#   - Setup test labels and test file list (TODO).
#   - Copy filelist.

set -o errexit
set -o nounset
set -o pipefail

if [[ $# -ne 6 ]]; then
    echo "./utils/setup_merlin.sh <FESTIVAL_VOICE_PATH> <ABSOLUTE_MERLIN_PATH> <WAV_PATH> <LANGUAGE> \
<LOCALE> <SAMPLE_RATE>"
    exit 1
fi

# Setup environment variables.
export FESTIVAL_VOICE_PATH=$1
export MERLIN_PATH=$2
export WAV_PATH=$3
BASEDIR=$(dirname "$0")
LANGUAGE=$4
LOCALE=$5
SAMPLE_RATE=$6

LANGUAGE_RESOURCE_PATH=$(pwd)

export MERLIN_VOICE_PATH="${MERLIN_PATH}/egs/${LOCALE}/s1/"
export MERLIN_DATA_PATH="${MERLIN_VOICE_PATH}/data"

# TODO(pasindu): Make this an argument.
FESTIVAL_VOICE_FILE_LIST_PATH="${FESTIVAL_VOICE_PATH}/file_id_list.scp"

echo "FESTIVAL_VOICE_PATH - ${FESTIVAL_VOICE_PATH:?Set env variable FESTIVAL_VOICE_PATH}"
echo "MERLIN_PATH - ${MERLIN_PATH:?Set env variable MERLIN_PATH}"
echo "WAV_PATH - ${WAV_PATH:?Set env variable WAV_PATH}"
echo "LANGUAGE_RESOURCE_PATH - ${LANGUAGE_RESOURCE_PATH:?Set env variable LANGUAGE_RESOURCE_PATH}"

echo "Clean files from the previous run"
rm -rf label_state_align/
rm -rf label_no_align/
rm -rf mono_no_align/
rm -rf config/
rm -rf model/
rm -rf mfc/
rm -f mono_align.mlf
rm -f mono_phone.list
rm -f phoneme_map.dict
rm -rf prompt-lab/full/
rm -rf prompt-lab/mono

MERLIN_DATA_PATH_WAV="${MERLIN_DATA_PATH}/wav"

echo "Create directories required by Merlin."
mkdir -p "${MERLIN_DATA_PATH}"
mkdir -p "${MERLIN_DATA_PATH}/question"
mkdir -p "${MERLIN_VOICE_PATH}/conf"
mkdir -p "${MERLIN_DATA_PATH}/label_state_align"
mkdir -p "${MERLIN_DATA_PATH}/label_phone_align"
mkdir -p "${MERLIN_DATA_PATH_WAV}"

# Compile Merlin tools.
if [ ! -f "${MERLIN_PATH}/tools/bin/WORLD/analysis" ]
then
  echo "Compile Merlin tools."
  cd "${MERLIN_PATH}/tools"
  sh compile_tools.sh
  # Go back to the directory we came from.
  cd "${LANGUAGE_RESOURCE_PATH}"
fi

# TODO(pasindu): Make this a argument.
echo "Copying wav from festival to Merlin."
cp "${FESTIVAL_VOICE_PATH}"/wav/*.wav "${MERLIN_DATA_PATH_WAV}"

echo "Extracting acoustic features."
python "${MERLIN_PATH}/misc/scripts/vocoder/world/extract_features_for_merlin.py" "${MERLIN_PATH}" \
  "${WAV_PATH}" "${MERLIN_DATA_PATH}" "${SAMPLE_RATE}"

echo "Preparing to generate HTS labels files from the given festival voice."
TMP_MERLIN_SCRIPTS_PATH="/tmp/merlin_scripts/"

# Clean files from the previous run.
rm -rf "${TMP_MERLIN_SCRIPTS_PATH}"
mkdir -p "${TMP_MERLIN_SCRIPTS_PATH}"
cp -R -T "${MERLIN_PATH}/misc/scripts/frontend/festival_utt_to_lab" "${TMP_MERLIN_SCRIPTS_PATH}"


# Find the name of festival phoneset file.
PHONESET_FILE_PATH=$(find "$FESTIVAL_VOICE_PATH"/festvox/*phoneset.scm)
LOAD_PHONESET_SCRIPT="(load \"$PHONESET_FILE_PATH\") "

# Load phoneme set before generating the labels.
# This adds a lisp cmd to load the current festival setup's phoneset to the 49th line
# of the extra_feats.scm (which is between the license and start of the code) . This is
# because extra_feats.scm defaults to the radio phoneset.
#
# TODO(pasindu): Make this cleaner.
sed -i  '49s|$|'"$LOAD_PHONESET_SCRIPT"'|' \
      "${TMP_MERLIN_SCRIPTS_PATH}/extra_feats.scm"

FESTIVAL_LABEL_PHONE_ALIGN="${FESTIVAL_VOICE_PATH}/prompt-lab/full"

echo "Generating labels."
${TMP_MERLIN_SCRIPTS_PATH}/make_labels \
  "${FESTIVAL_VOICE_PATH}/prompt-lab" \
  "${FESTIVAL_VOICE_PATH}/festival/utts" \
  "${FESTIVALDIR}/examples/dumpfeats" \
  "${TMP_MERLIN_SCRIPTS_PATH}"

# Remove leading space from the label files.
sed -i -r "s/^ *//g" "${FESTIVAL_LABEL_PHONE_ALIGN}"/*.lab
# Delimiter lines with single space.
sed -i -r "s/  */ /g" "${FESTIVAL_LABEL_PHONE_ALIGN}"/*.lab

# TODO(pasindu): Run a script to normalize the labels.

# TODO(pasindu): Run a script to generate Merlin state labels.

MERLIN_LABEL_PHONE_ALIGN="${MERLIN_DATA_PATH}/label_phone_align"

echo "Copying phone labels to ${MERLIN_LABEL_PHONE_ALIGN}"
cp "${FESTIVAL_LABEL_PHONE_ALIGN}"/*.lab "${MERLIN_LABEL_PHONE_ALIGN}"

# TODO(pasindu): Check in these files for each langauge.
echo "Generating HTS questions"
${BASEDIR}/generate_hts_questions.py "${LANGUAGE}/festvox/ipa_phonology.json" \
  > "${MERLIN_DATA_PATH}/question/auto_generated_questions.hed"

# TODO(pasindu): Replace this with a python script.
echo  "Setting up Merlin configuration files"
ACOUSTIC_CONF_FILE="${MERLIN_VOICE_PATH}/conf/acoustic_dnn.conf"
DURATION_CONF_FILE="${MERLIN_VOICE_PATH}/conf/duration_dnn.conf"
TEST_DURATION_CONF_FILE="${MERLIN_VOICE_PATH}/conf/test_dur_synth.conf"
TEST_ACOUSTIC_CONF_FILE="${MERLIN_VOICE_PATH}/conf/test_synth.conf"

echo "Copying conf files."
cp ${BASEDIR}/../third_party/merlin/acoustic_dnn.conf "${ACOUSTIC_CONF_FILE}"
cp ${BASEDIR}/../third_party/merlin/duration_dnn.conf "${DURATION_CONF_FILE}"
cp ${BASEDIR}/../third_party/merlin/test_dur_synth.conf "${TEST_DURATION_CONF_FILE}"
cp ${BASEDIR}/../third_party/merlin/test_synth.conf "${TEST_ACOUSTIC_CONF_FILE}"

echo "ACOUSTIC_CONF_FILE - ${ACOUSTIC_CONF_FILE}"
echo "DURATION_CONF_FILE - ${DURATION_CONF_FILE}"
echo "TEST_DURATION_CONF_FILE - ${TEST_DURATION_CONF_FILE}"
echo "TEST_ACOUSTIC_CONF_FILE - ${TEST_ACOUSTIC_CONF_FILE}"

sed -i "s|MERLIN_PATH|${MERLIN_PATH}|g" "${ACOUSTIC_CONF_FILE}"
sed -i "s|MERLIN_TOPLEVEL_PATH|${MERLIN_VOICE_PATH}|g" "${ACOUSTIC_CONF_FILE}"
sed -i "s|SAMPLE_RATE|${SAMPLE_RATE}|g" "${ACOUSTIC_CONF_FILE}"

sed -i "s|MERLIN_PATH|${MERLIN_PATH}|g" "${DURATION_CONF_FILE}"
sed -i "s|MERLIN_TOPLEVEL_PATH|${MERLIN_VOICE_PATH}|g" "${DURATION_CONF_FILE}"
sed -i "s|SAMPLE_RATE|${SAMPLE_RATE}|g" "${DURATION_CONF_FILE}"

sed -i "s|MERLIN_PATH|${MERLIN_PATH}|g" "${TEST_DURATION_CONF_FILE}"
sed -i "s|MERLIN_TOPLEVEL_PATH|${MERLIN_VOICE_PATH}|g" "${TEST_DURATION_CONF_FILE}"

sed -i "s|MERLIN_PATH|${MERLIN_PATH}|g" "${TEST_ACOUSTIC_CONF_FILE}"
sed -i "s|MERLIN_TOPLEVEL_PATH|${MERLIN_VOICE_PATH}|g" "${TEST_ACOUSTIC_CONF_FILE}"
sed -i "s|SAMPLE_RATE|${SAMPLE_RATE}|g" "${TEST_ACOUSTIC_CONF_FILE}"

MERLIN_FILE_LIST_PATH="${MERLIN_DATA_PATH}/file_id_list.scp"
echo "Copying filelist ${FESTIVAL_VOICE_FILE_LIST_PATH}."
cp "${FESTIVAL_VOICE_FILE_LIST_PATH}"  "${MERLIN_FILE_LIST_PATH}"

# TODO(pasindu): Run script to validate the setup.

# TODO(pasindu): Generate the features and ensure that they are within the error limits.

echo "Setup Done."
