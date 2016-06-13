#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

order="${1:-3}"
theta="${2:-0}"

projdir="${0}.runfiles"
openfst="${0}.runfiles/external/openfst"
opengrm="${0}.runfiles/external/opengrm_ngram"

"$projdir/my/extract_text.py" |
"$projdir/my/text_to_symbols.py" \
  "$projdir/my/codepoint.syms" |
"$openfst/farcompilestrings" \
  --symbols="$projdir/my/codepoint.syms" \
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
