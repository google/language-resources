#! /bin/bash
#
# Check that utf8_test.py produces identical UTF-8 output irrespective
# of Python version and environment settings.

set -o nounset

status=0

for py in /usr/bin/python{,2.6,2.7} python{,2,2.6,2.7,3,3.{3..7}}; do
  echo
  echo $py
  py_binary="$(which $py)"
  if [ -n "$py_binary" ] && "$py_binary" -c pass; then
    echo "$py_binary"
    "$py_binary" --version
    for e in '' '-' 'LC_CTYPE=C' 'PYTHONIOENCODING=ASCII' \
      'PYTHONIOENCODING=Latin-1'; do
      env $e "$py_binary" -c 'import sys; print(sys.stdout.encoding)'
      env $e "$py_binary" "$1" | diff -u - "$2" || status=1
    done
  fi
done

exit $status
