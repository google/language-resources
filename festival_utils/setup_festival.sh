#!/bin/bash
#
# Setup festival tools.

cd "${FESTIVAL_SUIT_PATH}"
echo "Setting up at festival at ${FESTIVAL_SUIT_PATH:?Set env variable FESTIVAL_SUIT_PATH}"

echo "Downloading up festival and friends."
curl -L http://festvox.org/packed/festival/2.5/festlex_CMU.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
curl -L http://festvox.org/packed/festival/2.5/festlex_POSLEX.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
curl -L http://festvox.org/packed/festival/2.5/voices/festvox_kallpc16k.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
curl -L http://festvox.org/packed/festvox/2.8/festvox-2.8.0-release.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
curl -L https://download.sourceforge.net/project/sp-tk/SPTK/SPTK-3.11/SPTK-3.11.tar.gz | \
tar xz --no-same-owner --no-same-permissions && \
patch -p0 < festvox/src/clustergen/SPTK-3.6.patch
curl -L http://festvox.org/packed/festival/2.5/speech_tools-2.5.0-release.tar.gz | \
tar xz --no-same-owner --no-same-permissions
curl -L http://festvox.org/packed/festival/2.5/festival-2.5.0-release.tar.gz | \
tar xz --no-same-owner --no-same-permissions
curl -L http://festvox.org/flite/packed/flite-2.1/flite-2.1-release.tar.bz2 | \
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

# Build Flite
cd "${FESTIVAL_SUIT_PATH}/flite"
./configure && make

echo "Please add the following lines to your environment setting (.bashrc, .zshrc, etc.)"
echo "export ESTDIR=${FESTIVAL_SUIT_PATH}/speech_tools"
echo "export FESTVOXDIR=${FESTIVAL_SUIT_PATH}/festvox"
echo "export FESTIVALDIR=${FESTIVAL_SUIT_PATH}/festival"
echo "export FLITEDIR=${FESTIVAL_SUIT_PATH}/flite"
echo "export SPTKDIR=${FESTIVAL_SUIT_PATH}/SPTK"
echo "export EXDIR=${FESTIVAL_SUIT_PATH}/festival/examples"
