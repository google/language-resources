#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

projdir="${0}.runfiles"

exec "$projdir/utils/lm-scores" \
  "$projdir/my/models/unicode.fst" \
  "$projdir/my/models/zawgyi.fst" |
awk '
  BEGIN {
    FS = "\t"
    OFS = "\t"
  }

  {
    n2 = split($2, component2, "/")
    n4 = split($4, component4, "/")
    print $1, component2[n2], $3, component4[n4], $5
  }
'
