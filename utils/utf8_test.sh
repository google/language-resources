#! /bin/bash
#
# Check that utf8_test.py produces identical UTF-8 output irrespective
# of Python version and environment settings.

set -o errexit
set -o nounset

for py in python python2 python3; do
  if which $py; then
    for e in '' '-' 'LC_CTYPE=C' 'PYTHONIOENCODING=ASCII' \
      'PYTHONIOENCODING=Latin-1'; do
      env $e $py -c 'import sys; print(sys.stdout.encoding)'
      env $e $py "$1" | diff -u - "$2"
    done
  fi
done
