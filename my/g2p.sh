#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

runfiles="${0}.runfiles"

"$runfiles/utils/thrax_g2p" \
  --fst="$runfiles/my/G2P.fst" \
  --phoneme_syms="$runfiles/my/ipa_phoneme.syms"
