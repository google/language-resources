#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

if [ "$#" -ne 1 ]; then
  echo "Usage: $0 OUTPUT_DIRECTORY" >&2
  exit 2
fi

outdir="$1"

# Output files that will be generated in $outdir:
syms="$outdir/graphone.syms"
train="$outdir/g2p_train.far"
test="$outdir/g2p_test.tsv"

# Input files and tools:
runfiles="${0}.runfiles"
dict="$runfiles/af/lex_regular.txt"
alignables="$runfiles/af/alignables.txt"
words="$runfiles/af/test_words.txt"
festus="$runfiles/festus"
openfst="$runfiles/external/openfst"

export LC_ALL=C

"$festus/make-alignable-symbols" \
  --alignables="$alignables" \
  "$syms"

sort -c "$words"

sort -t$'\t' -k1,1 "$dict" |
join -t$'\t' -j1 "$words" - \
  > "$test"

sort -t$'\t' -k1,1 "$dict" |
comm -23 - "$test" |
"$festus/lexicon-diagnostics" \
  --alignables="$alignables" \
  --unique_alignments \
  --filter |
tee "$outdir/g2p_train.tsv" |
cut -f 3 |
"$openfst/farcompilestrings" \
  --symbols="$syms" \
  --keep_symbols \
  --generate_keys=6 \
  > "$train"
