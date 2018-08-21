#export KALDI_ROOT=~/govivace/spkinfo/kaldi
export KALDI_ROOT=$(cd ../../../../../../; pwd -P)
[ -f $KALDI_ROOT/tools/env.sh ] && . $KALDI_ROOT/tools/env.sh
export PATH=$PWD/utils/:$KALDI_ROOT/tools/openfst/bin:$PWD:$PATH
[ ! -f $KALDI_ROOT/tools/config/common_path.sh ] && echo >&2 "The standard file $KALDI_ROOT/tools/config/common_path.sh is not present -> Exit!" && exit 1
. $KALDI_ROOT/tools/config/common_path.sh
export LC_ALL=C
export SPARROWHAWK_ROOT=$KALDI_ROOT/tools/sparrowhawk
export PATH=$SPARROWHAWK_ROOT/bin:$PATH
export PATH=$KALDI_ROOT/tools/thrax/bin:$PATH
