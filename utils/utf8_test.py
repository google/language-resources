from __future__ import print_function

import sys

import utf8


def test_print(p):
  p(u'hello', u'world')
  p(u'hello', b'world')
  p(b'hello', u'world')
  p(b'hello', b'world')

  # Test byte strings which are valid UTF-8 byte sequences:
  p(u'\xa1Hola!', u'\xa1mundo!')
  p(u'\xa1Hola!', b'\xc2\xa1mundo!')
  p(b'\xc2\xa1Hola!', u'\xa1mundo!')
  p(b'\xc2\xa1Hola!', b'\xc2\xa1mundo!')

  # Test an invalid UTF-8 byte sequence:
  p(b'\xa1', b'Hola!')
  return


def main(argv):
  if len(argv) > 1 and argv[1].startswith('p'):
    def p(*values):
      print(*values)
  else:
    def p(*values):
      utf8.Print(*values)
  test_print(p)
  return


if __name__ == '__main__':
  main(sys.argv)
