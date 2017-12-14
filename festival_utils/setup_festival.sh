#!/bin/bash
#
# Setup festival tools.

cd "${FESTIVAL_SUIT_PATH}"
echo "Setting up at festival at ${FESTIVAL_SUIT_PATH:?Set env variable FESTIVAL_SUIT_PATH}"

echo "Downloading up festival and friends."
curl -L http://festvox.org/packed/festival/2.4/festlex_CMU.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
curl -L http://festvox.org/packed/festival/2.4/festlex_POSLEX.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
curl -L http://festvox.org/packed/festival/2.4/voices/festvox_kallpc16k.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
curl -L http://festvox.org/festvox-2.7/festvox-2.7.0-release.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
curl -L http://tts.speech.cs.cmu.edu/awb/sptk/SPTK-3.6.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
patch -p0 < festvox/src/clustergen/SPTK-3.6.patch
curl -L http://festvox.org/packed/festival/2.4/speech_tools-2.4-release.tar.gz | \
tar xz --no-same-owner --no-same-permissions
curl -L http://festvox.org/packed/festival/2.4/festival-2.4-release.tar.gz | \
tar xz --no-same-owner --no-same-permissions

echo "Building festival suit."

# Build and install SPTK
cd "${FESTIVAL_SUIT_PATH}/SPTK-3.6"
./configure --prefix="${FESTIVAL_SUIT_PATH}" && make && make install && make distclean

# Build the Edinburgh Speech Tools
cd "${FESTIVAL_SUIT_PATH}/speech_tools"
./configure && make && make install

# Build Festival
cd "${FESTIVAL_SUIT_PATH}/festival"
./configure && make && make install

# Build Festvox
cd "${FESTIVAL_SUIT_PATH}/festvox"
./configure && make

echo "Please add the following lines to your environment setting (.bashrc, .zshrc, etc.)"
echo "export ESTDIR=$(pwd)/speech_tools"
echo "export FESTVOXDIR=$(pwd)/festvox"
echo "export FESTIVALDIR=$(pwd)/festival"
echo "export FLITEDIR=$(pwd)/flite"
echo "export SPTKDIR=$(pwd)/SPTK"
echo "export EXDIR=$(pwd)/examples"
