#! /bin/bash

set -o errexit
set -o nounset

if [ "$#" -eq 0 ]; then
  echo "Usage: $0 model_directory [word_list]" >&2
  exit 2
fi

dir="$(dirname $0)"
model="$1"
shift

if [ ! -d "$dir/models/$model" ]; then
  echo "$model is not a directory under $dir/models/" >&2
  exit 2
fi

modeldir="$dir/models/$model"

exec "$dir/runtime/g2p-lookup" \
  --bytes_to_graphones="$modeldir/bytes_to_graphones.fst" \
  --graphone_model="$modeldir/graphone_model.fst" \
  --phonemes_to_graphones="$modeldir/phonemes_to_graphones.fst" \
  "$@"
