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

from __future__ import print_function

import sys

# Special unqualified local import, for use with utf8_test.sh:
import utf8


class Foo(object):

  def __str__(self):
    return 'Foo'


def test_print(p):
  p(True, 42, 3.14, None, [], Foo())

  p(u'hello', u'world')
  p(u'hello', b'world')
  p(b'hello', u'world')
  p(b'hello', b'world')

  # Test byte strings which are valid UTF-8 byte sequences:
  p(u'\xa1Hola!', u'\xa1mundo!')
  p(u'\xa1Hola!', b'\xc2\xa1mundo!')
  p(b'\xc2\xa1Hola!', u'\xa1mundo!')
  p(b'\xc2\xa1Hola!', b'\xc2\xa1mundo!')

  # Test invalid UTF-8 byte sequences:
  p(b'\xc2Hola!', u'\xa1mundo!')
  p(b'\xa1Hola!', u'\xa1mundo!')
  return


def main(argv):
  if len(argv) > 1 and argv[1].startswith('p'):
    def p(*values):
      print(*values)
  else:
    def p(*values):
      utf8.Print(*values)
  test_print(p)
  sys.stdout.flush()
  return


if __name__ == '__main__':
  main(sys.argv)
