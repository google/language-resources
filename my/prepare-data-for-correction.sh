#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

runfiles="${0}.runfiles"

G2P () {
  cut -f 1 |
  "$runfiles/utils/thrax_g2p" \
    --fst="$runfiles/my/G2P.fst" \
    --phoneme_syms="$runfiles/my/custom_phoneme.syms"
}

G2P |
fgrep -v ERROR |
"$runfiles/my/split_sentences.py" 130 |
G2P |
awk -F"\t" '{printf "%s\n%s\n%s\n\n", $1, $2, $2}' |
tr \| /
