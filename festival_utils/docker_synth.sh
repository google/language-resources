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

set -v
set -x
set -e

# Read text from stdin
read txt; TEXT=${txt};

# Unique synthesizer ID. 
SYNTH_ID=$RANDOM

# Call synth.sh to generate wav.
echo "${txt}" | /usr/local/src/language-resources/docker-images/test-merlin/synth.sh "${SYNTH_ID}"

# Stream back the generated wav.
cat "/usr/local/src/merlin/egs/locale/s1/models/test_synthesis/wav/${SYNTH_ID}.wav"
