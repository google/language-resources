# Copyright 2017 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

r"""Simplified text I/O module that hard-wires UTF-8 text encoding.

Typical use case:

>>> with utf8.open('/tmp/foo', 'w') as f:
...   utf8.Print(u'\u00A1Hola, mundo!', file=f)

This is to work around two kinds of issues in Python:

* Limited compatibility between Python 2 and 3 when it comes to Unicode string
  handling, especially concerning I/O. This module is written to work directly
  with both Python 2 and 3, without the need for 2to3 conversion or the 'six'
  compatibility module.

* Unexpected default behavior of text I/O in the Python standard library across
  all versions. The problem is that the standard library is trying a bit too
  hard to adapt its I/O behavior to its environment, as specified by environment
  variables, terminal properties, etc. This makes it difficult to predict the
  behavior of a program in isolation. E.g. seemingly straightforward and docile
  scripts can behave differently when stdout is connected to a terminal
  vs. piped into a pager. Tests can behave differently when run in a user's
  shell environment vs. a sterile test environemnt that clears out most
  environment variables.

The present module aims to address both issues by providing drop-in replacements
for well-known Python functions and objects. Only one encoding is supported,
which is fixed to UTF-8, cannot be passed as an argument, and is not inferred
from the environment.

The replacement for the Python print() function (the default in Python 3 and
available via __future__.print_function in Python 2) is called Print()
here. This is done in order to avoid syntax errors when used in Python 2 legacy
code when 'print' cannot be used a function name. If a lower-case function name
print() is desired, this can be done by definining:

>>> from __future__ import print_function
>>> def print(*objects, **kwargs):
...   Print(*objects, **kwargs)

The import of print_function ensures that 'print' parses as an identifier rather
than a keyword, so that it can be used as a function name.
"""

import io

encoding = 'UTF-8'

stdin = io.open(0, mode='rt', encoding=encoding, closefd=False)

stdout = io.open(1, mode='wt', encoding=encoding, closefd=False)

stderr = io.open(2, mode='wt', encoding=encoding, closefd=False)


def open(filename, mode='r'):  # pylint: disable=redefined-builtin
  """Open file and return a textual stream."""
  if mode == 'r':
    return io.open(filename, mode='rt', encoding=encoding)
  elif mode == 'w':
    return io.open(filename, mode='wt', encoding=encoding)
  elif mode == 'a':
    return io.open(filename, mode='at', encoding=encoding)
  else:
    raise ValueError("invalid mode: '%s'" % mode)
  return


def GetContents(filename):  # pylint: disable=invalid-name
  with open(filename) as reader:
    contents = reader.read()
  return contents


def _str(value, depth=0):
  assert depth <= 2
  # The recommended style of isinstance() would not be portable, because the
  # type names of u'' and b'' differ between Python 2 and 3.
  if type(value) == type(u''):  # pylint: disable=unidiomatic-typecheck
    return value
  elif type(value) == type(b''):  # pylint: disable=unidiomatic-typecheck
    return value.decode(encoding, 'replace')
  else:
    return _str(str(value), depth + 1)


def Print(*values, **kwargs):  # pylint: disable=invalid-name
  """Print values to a textual stream, or stdout by default."""
  sep = kwargs.get('sep', u' ')
  end = kwargs.get('end', u'\n')
  f = kwargs.get('file', stdout)
  s = sep.join(_str(v) for v in values)
  s += end
  f.write(s)
  if kwargs.get('flush', False):
    f.flush()
  return


if __name__ == '__main__':
  for line in stdin:
    Print(line, end='', flush=True)
