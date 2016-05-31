#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

order="${1:-3}"
theta="${2:-0}"

runfiles="${0}.runfiles"

"$runfiles/my/extract_text.py" |
"$runfiles/my/text_to_symbols.py" |
"$runfiles/external/openfst/farcompilestrings" \
  --symbols="$runfiles/my/codepoint.syms" \
  --keep_symbols \
  --generate_keys=6 |
"$runfiles/external/opengrm_ngram/ngramcount" \
  --order="$order" |
"$runfiles/external/opengrm_ngram/ngrammake" \
  --check_consistency \
  --method=kneser_ney \
  --backoff |
"$runfiles/external/opengrm_ngram/ngramshrink" \
  --check_consistency \
  --method=seymore \
  --shrink_opt=2 \
  --theta="$theta"
