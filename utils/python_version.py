#! /usr/bin/env python

"""Print Python interpreter path, version and Unicode-related diagnostics."""

from __future__ import unicode_literals

import sys

sys.stdout.write('executable: %s\n' % sys.executable)
sys.stdout.write('version: %s\n' % sys.version)
sys.stdout.write('stdout encoding: %s\n' % sys.stdout.encoding)

if sys.maxunicode == 0xFFFF:
  sys.stdout.write('--enable-unicode=ucs2 ("narrow")\n')
elif sys.maxunicode == 0x10FFFF:
  sys.stdout.write('--enable-unicode=ucs4 ("wide")\n')
else:
  sys.stdout.write('Unknown Unicode build; this cannot happen\n')

try:
  import icu  # pylint: disable=g-import-not-at-top
  sys.stdout.write('Unicode %s (ICU %s) via module icu %s\n' %
                   (icu.UNICODE_VERSION, icu.ICU_VERSION, icu.VERSION))
  assert icu.Char.charName('\U00010300') == 'OLD ITALIC LETTER A'
except ImportError:
  pass

import unicodedata  # pylint: disable=g-import-not-at-top
sys.stdout.write('Unicode %s via module unicodedata\n' %
                 unicodedata.unidata_version)
assert unicodedata.name('\U00010300') == 'OLD ITALIC LETTER A'
