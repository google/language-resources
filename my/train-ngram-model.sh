#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

symbols="$1"
order="${2:-3}"
theta="${3:-0}"

projdir="${0}.runfiles/language_resources"
openfst="${0}.runfiles/openfst"
opengrm="${0}.runfiles/opengrm_ngram"

"$projdir/my/text_to_symbols.py" \
  "$symbols" |
"$openfst/farcompilestrings" \
  --symbols="$symbols" \
  --keep_symbols \
  --generate_keys=7 |
"$opengrm/ngramcount" \
  --order="$order" |
"$opengrm/ngrammake" \
  --check_consistency \
  --method=kneser_ney \
  --backoff |
"$opengrm/ngramshrink" \
  --check_consistency \
  --method=seymore \
  --shrink_opt=2 \
  --theta="$theta"
