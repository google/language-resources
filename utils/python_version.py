#! /usr/bin/env python

"""Print Python interpreter path and version."""

import sys

sys.stdout.write(sys.executable + '\n')
sys.stdout.write(sys.version + '\n')
sys.stdout.flush()
