#! /usr/bin/env python

"""Print Python interpreter path and version."""

import sys

sys.stdout.write('%s\n' % sys.executable)
sys.stdout.write('%s\n' % sys.version)

try:
  import icu  # pylint: disable=g-import-not-at-top
  sys.stdout.write('ICU %s\n' % icu.ICU_VERSION)
  sys.stdout.write('Unicode %s\n' % icu.UNICODE_VERSION)
except ImportError:
  pass

sys.stdout.flush()
