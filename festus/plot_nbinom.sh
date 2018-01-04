#! /bin/bash

PROJDIR="$0.runfiles/language_resources"

SIZE="${1:-5}"
MU="${2:-10}"
PLOT="${3:-/tmp/nbinom_hist.pdf}"
HISTOGRAM=/tmp/nbinom_hist.dat

PATH="$PROJDIR/festus:$PROJDIR/external/openfst:$PATH"

fstnbinom "$SIZE" "$MU" |
  fstrandgen --select=log_prob --weighted --npath=2000 |
  fstrmepsilon |
  fstdeterminize |
  fstminimize |
  fstpush --push_weights --to_final --remove_total_weight |
  fstprint |
  awk 'NF <= 2 {print $1, exp(-$2)}' > "$HISTOGRAM"

echo "
  h <- read.table('$HISTOGRAM')
  pdf('$PLOT')
  plot(h, type='h', col='blue', lwd=5)
  lines(h\$V1, dnbinom(h\$V1, size=$SIZE, mu=$MU), type='l', col='red', lwd=3)
  dev.off()
" |
  R --vanilla --quiet

ls -l "$PLOT"
