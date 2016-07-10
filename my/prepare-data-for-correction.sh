#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

projdir="${0}.runfiles/language_resources"

G2P () {
  cut -f 1 |
  "$projdir/my/tokenize.py" |
  "$projdir/utils/thrax_g2p" \
    --fst="$projdir/my/G2P.fst" \
    --phoneme_syms="$projdir/my/custom_phoneme.syms"
}

G2P |
fgrep -v ERROR |
"$projdir/my/split_sentences.py" 130 |
G2P |
awk -F"\t" '{printf "%s\n%s\n%s\n\n", $1, $2, $2}' |
tr \| /
