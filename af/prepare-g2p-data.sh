#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

if [ "$#" -ne 4 ]; then
  echo "Usage: $0 DICTIONARY ALIGNABLES TEST_WORDS OUTPUT_DIRECTORY" >&2
  exit 2
fi

# Input files and output directory:
dict="$1"
alignables="$2"
words="$3"
outdir="$4"

# Output files that will be generated in $outdir:
syms="$outdir/graphone.syms"
train="$outdir/g2p_train.far"
test="$outdir/g2p_test.tsv"

# Tools:
runfiles="${0}.runfiles"
festus="$runfiles/language_resources/festus"
openfst="$runfiles/openfst"

export LC_ALL=C.UTF-8

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
