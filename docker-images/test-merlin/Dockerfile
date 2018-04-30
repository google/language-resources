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

FROM langtech/base-merlin:v1_1

WORKDIR /usr/local/src/voice/
ADD synth.sh .

WORKDIR /usr/local/src/voice/data/festvox

# Setup resources needed
RUN cp /usr/local/src/language-resources/si/festvox/lexicon.scm .
RUN cp /usr/local/src/language-resources/si/festvox/txt.done.data .
# Test only one speaker
RUN sed -i '/sin_9228*/!d' txt.done.data
RUN cp /usr/local/src/language-resources/si/festvox/ipa_phonology.json .

RUN mkdir -p /usr/local/src/voice/data/festvox/
RUN echo "{}" > /usr/local/src/voice/data/festvox/voiceSpec.json

# Download sinhala wavs
RUN mkdir wavs && cd wavs && wget http://www.openslr.org/resources/30/si_lk.tar.gz && \
tar -xf si_lk.tar.gz && \
find . -type f -not -name 'sin_9228*' -print0 | xargs -0 rm --
