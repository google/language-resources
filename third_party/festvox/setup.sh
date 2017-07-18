#!/bin/sh
###########################################################################
##                                                                       ##
##                     Carnegie Mellon University                        ##
##                         Copyright (c) 2014                            ##
##                        All Rights Reserved.                           ##
##                                                                       ##
##  Permission is hereby granted, free of charge, to use and distribute  ##
##  this software and its documentation without restriction, including   ##
##  without limitation the rights to use, copy, modify, merge, publish,  ##
##  distribute, sublicense, and/or sell copies of this work, and to      ##
##  permit persons to whom this work is furnished to do so, subject to   ##
##  the following conditions:                                            ##
##   1. The code must retain the above copyright notice, this list of    ##
##      conditions and the following disclaimer.                         ##
##   2. Any modifications must be clearly marked as such.                ##
##   3. Original authors' names are not deleted.                         ##
##   4. The authors' names are not used to endorse or promote products   ##
##      derived from this software without specific prior written        ##
##      permission.                                                      ##
##                                                                       ##
##  CARNEGIE MELLON UNIVERSITY AND THE CONTRIBUTORS TO THIS WORK         ##
##  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING      ##
##  ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT   ##
##  SHALL CARNEGIE MELLON UNIVERSITY NOR THE CONTRIBUTORS BE LIABLE      ##
##  FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES    ##
##  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN   ##
##  AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,          ##
##  ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF       ##
##  THIS SOFTWARE.                                                       ##
##                                                                       ##
###########################################################################
##                                                                       ##
##  Example script used to test the Festival 2.4/Flite 2.0 release       ##
##                                                                       ##
##  Downloads code, compiles it, runs the voices, and builds a voice     ##
##                                                                       ##
###########################################################################

# Modification - To setup tools at a given directory
cd $FESTIVAL_SUIT_PATH
echo "Setting up at festival at ${FESTIVAL_SUIT_PATH:?Set env variable FESTIVAL_SUIT_PATH}"

# Download the code and voices, if not already downloaded
if [ ! -d packed ]
then

mkdir packed
cd packed
wget http://festvox.org/packed/festival/2.4/festival-2.4-release.tar.gz
wget http://festvox.org/packed/festival/2.4/speech_tools-2.4-release.tar.gz
wget http://festvox.org/packed/festival/2.4/festlex_CMU.tar.gz
wget http://festvox.org/packed/festival/2.4/festlex_OALD.tar.gz
wget http://festvox.org/packed/festival/2.4/festlex_POSLEX.tar.gz

wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_ahw_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_aup_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_awb_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_axb_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_bdl_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_clb_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_fem_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_gka_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_jmk_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_ksp_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_rms_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_rxr_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_cmu_us_slt_cg.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_kallpc16k.tar.gz
wget http://festvox.org/packed/festival/2.4/voices/festvox_rablpc16k.tar.gz

wget http://festvox.org/festvox-2.7/festvox-2.7.0-release.tar.gz
wget http://tts.speech.cs.cmu.edu/awb/sptk/SPTK-3.6.tar.gz

wget http://festvox.org/flite/packed/flite-2.0/flite-2.0.0-release.tar.bz2

wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_ahw.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_aup.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_awb.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_axb.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_bdl.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_clb.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_fem.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_gka.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_jmk.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_ksp.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_rms.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_rxr.flitevox
wget http://festvox.org/flite/packed/flite-2.0/voices/cmu_us_slt.flitevox

cd ..

fi

# Unpack the code and voices
mkdir build
cd build

for i in ../packed/*.gz
do
   tar zxvf $i
done

for i in ../packed/*.bz2
do
   tar jxvf $i
done
mv flite-2.0.0-release flite

# Set up the environment variables for voice building
export ESTDIR=`pwd`/speech_tools
export FESTVOXDIR=`pwd`/festvox
export FLITEDIR=`pwd`/flite
export SPTKDIR=`pwd`/SPTK
mkdir SPTK
export EXDIR=`pwd`/examples
mkdir examples

echo "A whole joy was reaping, but they've gone south, you should fetch azure mike." >allphones.txt

patch -p0 <festvox/src/clustergen/SPTK-3.6.patch
cd SPTK-3.6
./configure --prefix=$SPTKDIR
make
make install
cd ..

cd speech_tools
./configure
make
make test
cd ..

cd festival
./configure
make
make test

./bin/text2wave -eval '(voice_cmu_us_ahw_cg)' -o $EXDIR/festival_ahw_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_aup_cg)' -o $EXDIR/festival_aup_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_awb_cg)' -o $EXDIR/festival_awb_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_axb_cg)' -o $EXDIR/festival_axb_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_bdl_cg)' -o $EXDIR/festival_bdl_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_clb_cg)' -o $EXDIR/festival_clb_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_fem_cg)' -o $EXDIR/festival_fem_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_gka_cg)' -o $EXDIR/festival_gka_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_jmk_cg)' -o $EXDIR/festival_jmk_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_ksp_cg)' -o $EXDIR/festival_ksp_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_rms_cg)' -o $EXDIR/festival_rms_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_rxr_cg)' -o $EXDIR/festival_rxr_hw.wav ../allphones.txt
./bin/text2wave -eval '(voice_cmu_us_slt_cg)' -o $EXDIR/festival_slt_hw.wav ../allphones.txt

cd ..

cd flite
./configure
make

mkdir voices
cd voices
ln ../../../packed/*.flitevox .
cd ..

# Generate some waveform files with the different voices
./bin/flite -voice voices/cmu_us_ahw.flitevox -f ../allphones.txt $EXDIR/flite_ahw_hw.wav
./bin/flite -voice voices/cmu_us_aup.flitevox -f ../allphones.txt $EXDIR/flite_aup_hw.wav
./bin/flite -voice voices/cmu_us_awb.flitevox -f ../allphones.txt $EXDIR/flite_awb_hw.wav
./bin/flite -voice voices/cmu_us_axb.flitevox -f ../allphones.txt $EXDIR/flite_axb_hw.wav
./bin/flite -voice voices/cmu_us_bdl.flitevox -f ../allphones.txt $EXDIR/flite_bdl_hw.wav
./bin/flite -voice voices/cmu_us_clb.flitevox -f ../allphones.txt $EXDIR/flite_clb_hw.wav
./bin/flite -voice voices/cmu_us_fem.flitevox -f ../allphones.txt $EXDIR/flite_fem_hw.wav
./bin/flite -voice voices/cmu_us_gka.flitevox -f ../allphones.txt $EXDIR/flite_gka_hw.wav
./bin/flite -voice voices/cmu_us_jmk.flitevox -f ../allphones.txt $EXDIR/flite_jmk_hw.wav
./bin/flite -voice voices/cmu_us_ksp.flitevox -f ../allphones.txt $EXDIR/flite_ksp_hw.wav
./bin/flite -voice voices/cmu_us_rms.flitevox -f ../allphones.txt $EXDIR/flite_rms_hw.wav
./bin/flite -voice voices/cmu_us_rxr.flitevox -f ../allphones.txt $EXDIR/flite_rxr_hw.wav
./bin/flite -voice voices/cmu_us_slt.flitevox -f ../allphones.txt $EXDIR/flite_slt_hw.wav
./bin/flite -voice kal -f ../allphones.txt $EXDIR/flite_kal_hw.wav

cd ..


# Voice building example (100 rms utts)
# Build festvox voice building tools
cd festvox
./configure
make
cd ..

mkdir cmu_us_rms100
cd cmu_us_rms100
$FESTVOXDIR/src/clustergen/setup_cg cmu us rms100
wget http://tts.speech.cs.cmu.edu/awb/short_arctic/rms100.tar.bz2
tar jxvf rms100.tar.bz2

# Build the voice (may take some time)
echo "Run ./bin/build_cg_rfs_voice to build the voice"


# Modification - To notice a user to export environment variables
echo "Please add the following lines to your environment setting (.bashrc, .zshrc, etc.)"
echo "export ESTDIR=`pwd`/speech_tools"
echo "export FESTVOXDIR=`pwd`/festvox"
echo "export FESTIVALDIR=`pwd`/festival"
echo "export FLITEDIR=`pwd`/flite"
echo "export SPTKDIR=`pwd`/SPTK"
echo "export EXDIR=`pwd`/examples"
