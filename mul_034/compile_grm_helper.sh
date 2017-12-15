#! /bin/bash

PROJDIR="$0.runfiles/language_resources"
FAR="$PWD/$2"

cd "$PROJDIR"
exec external/thrax/thraxcompiler --input_grammar="$1" --output_far="$FAR"
