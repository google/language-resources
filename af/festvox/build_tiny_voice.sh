#! /bin/bash
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
# Copyright 2016 Google, Inc.
# Author: pasindu@google.com (Pasindu De Silva)

set -o errexit
set -o nounset
set -o pipefail

LANG=af

runfiles="${0}.runfiles"
prompt_file="$runfiles/language_resources/af/festvox/txt.done.data"
apply_phonology="$runfiles/language_resources/utils/apply_phonology"
phonology_json="$runfiles/language_resources/af/festvox/phonology.json"
lexicon="$runfiles/language_resources/af/festvox/lexicon.scm"

if [ ! "$FESTVOXDIR" ]
then
   echo "Set env variable $FESTVOXDIR of festvox."
   exit 1
fi

if [ ! "$ESTDIR" ]
then
   echo "Set env variable $ESTDIR of speech tools."
   exit 1
fi

if [ ! "$SPTKDIR" ]
then
   echo "Set env variable $SPTKDIR of sptk."
   exit 1
fi

if [ ! "$PATH_TO_AF_WAVS" ]
then
   echo "Set path to the af wavs."
   exit 1
fi

# Set path to the wavs.
WAVS_PATH=${PATH_TO_AF_WAVS}

# Setup build directory.
VOICE_BUILD_DIR=$1
mkdir -p $VOICE_BUILD_DIR
cd $VOICE_BUILD_DIR

# Set up the Festvox Clustergen build.
$FESTVOXDIR/src/clustergen/setup_cg goog af unison

# Setup wavs.
rm -rf $VOICE_BUILD_DIR/wav
tar -C $VOICE_BUILD_DIR/ -xf $WAVS_PATH
mv $VOICE_BUILD_DIR/wavs $VOICE_BUILD_DIR/wav

# Setup prompts.
cp -fp $prompt_file  $VOICE_BUILD_DIR/etc/txt.done.data.orig

# Build voice from one user.
fgrep afr_8963_  $VOICE_BUILD_DIR/etc/txt.done.data.orig > $VOICE_BUILD_DIR/etc/txt.done.data

# Setup phonology and lexicon.
$apply_phonology $phonology_json $VOICE_BUILD_DIR
cp -fp $lexicon $VOICE_BUILD_DIR/festvox

# Configure a 22kHz voice:
sed -i 's/^(set! framerate .*$/(set! framerate 22050)/' festvox/clustergen.scm 

# Run the Festvox Clustergen build. This can take several minutes for every 100
# training prompts. Total running time depends heavily on the number of CPU
# cores available.
echo "Building afrikaan voice." 
./bin/build_cg_voice 
