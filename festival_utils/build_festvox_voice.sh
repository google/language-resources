#! /bin/bash
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
# Used to train festvox voices.
#
# Example usage -
#	./festival_utils/build_festvox_voice.sh ~/Desktop/audio/si_lk/ si ~/si_lk_2/

set -o errexit
set -o pipefail
set -o nounset

# Whether to run festvox TTS training.
TRAIN=true

while getopts ":t" opt; do
  case ${opt} in
    t ) # Option to disable training.
      TRAIN=false
      echo "Disable festvox training"
      ;;
  esac
done
shift $((OPTIND-1))

if [[ $# -ne 3 ]]; then
    echo "Usage: ./festival_utils/build_festvox_voice.sh <path to wavs> <lang> <voice_dir>"
    exit 1
fi


BASEDIR=$(dirname "$0")
PATH_TO_WAVS=$1
LANG=$2
VOICE_DIR=$3

# Check required env variables.
echo "${FESTVOXDIR?Set env variable FESTVOXDIR}"

# Set up the Festvox Clustergen build:
CWD=${PWD}
mkdir -p "${VOICE_DIR}"
cd "${VOICE_DIR}"
"${FESTVOXDIR}/src/clustergen/setup_cg" goog "${LANG}" unison

cd "${CWD}"

# Symlink wavs
rm -rf "${VOICE_DIR}/wav"
ln -sf "${PATH_TO_WAVS}" "${VOICE_DIR}/wav"

# Copy prompts
cp "${LANG}/festvox/txt.done.data" "${VOICE_DIR}/etc/"

# Copy festvox lexicon file.
cp "${LANG}/festvox/lexicon.scm" "${VOICE_DIR}/festvox/lexicon.scm"

# Setup the phonology.
PHONOLOGY="${LANG}/festvox/ipa_phonology.json"
if [ ! -f "${PHONOLOGY}" ];
then
  PHONOLOGY="${LANG}/festvox/phonology.json"
fi

# Generate various festvox files (wagon description files for (mcep,f0,dur), festvox phoneset etc).
python "${BASEDIR}/apply_phonology.py" "${PHONOLOGY}" "${VOICE_DIR}"

cd "${VOICE_DIR}"

if [[ ${TRAIN} == true ]]; then
  # Run the Festvox Clustergen build. This will take couple of hours to complete.
  # Total running time depends heavily on the number of CPU cores available.
  echo "Training festvox ${LANG} voice"
  time bin/build_cg_voice
fi
