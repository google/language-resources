#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

if [ "$#" -lt 2 ]; then
  echo "Usage: $0 train.far model.fst [order [theta]]" >&2
  exit 2
fi

# Input training data:
train="$1"

# Output model:
model="$2"

# Optional parameters:
order="${3:-6}"
theta="${4:-1e-2}"

# Tools:
runfiles="${0}.runfiles"
festus="$runfiles/festus"
opengrm="$runfiles/external/opengrm_ngram"

"$opengrm/ngramcount" \
  --order="$order" \
  "$train" |
"$opengrm/ngrammake" \
  --check_consistency \
  --method=kneser_ney \
  --backoff |
"$opengrm/ngramshrink" \
  --check_consistency \
  --method=seymore \
  --shrink_opt=2 \
  --theta="$theta" |
"$festus/ngramfinalize" \
  --phi_label=0 \
  --to_runtime_model \
  > "$model"
