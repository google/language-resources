#! /usr/bin/env python

"""Print Python interpreter path and version."""

import sys

sys.stdout.write('%s\n' % sys.executable)
sys.stdout.write('%s\n' % sys.version)

if sys.maxunicode == 0xFFFF:
  sys.stdout.write('Narrow (ucs2) Unicode build\n')
elif sys.maxunicode == 0x10FFFF:
  sys.stdout.write('Wide (ucs4) Unicode build\n')
else:
  sys.stdout.write('Unknown Unicode build\n')

try:
  import icu  # pylint: disable=g-import-not-at-top
  sys.stdout.write('Unicode %s (ICU %s) via module icu\n' %
                   (icu.UNICODE_VERSION, icu.ICU_VERSION))
except ImportError:
  import unicodedata  # pylint: disable=g-import-not-at-top
  sys.stdout.write('Unicode %s via module unicodedata\n' %
                   unicodedata.unidata_version)

sys.stdout.flush()
