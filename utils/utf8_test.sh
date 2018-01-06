#! /bin/bash
#
# Check that utf8_test.py produces identical UTF-8 output irrespective
# of Python version and environment settings.

set -o errexit
set -o nounset

for py in /usr/bin/python /usr/bin/python2.6 /usr/bin/python2.7 \
          python python2 python2.6 python2.7 python3; do
  if which $py; then
    py_binary="$(which $py)"
    for e in '' '-' 'LC_CTYPE=C' 'PYTHONIOENCODING=ASCII' \
      'PYTHONIOENCODING=Latin-1'; do
      env $e $py_binary -c 'import sys; print(sys.stdout.encoding)'
      env $e $py_binary "$1" | diff -u - "$2"
    done
  fi
done
