#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

projdir="${0}.runfiles"

exec "$projdir/utils/thrax_g2p" \
  --fst="$projdir/my/G2P.fst" \
  --phoneme_syms="$projdir/my/ipa_phoneme.syms"
