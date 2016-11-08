from __future__ import unicode_literals

import unittest

import utn11


class TestUtn11(unittest.TestCase):

  def TestCanonicalization(self):
    self.assertEqual('\u101B\u103E\u1031\u1037',
                     utn11.Canonicalize('\u101B\u103E\u1031\u1037'))
    self.assertEqual('\u101B\u103E\u1031\u1037',
                     utn11.Canonicalize('\u101B\u103E\u1037\u1031'))
    self.assertEqual('\u101B\u103E\u1031\u1037',
                     utn11.Canonicalize('\u101B\u1031\u103E\u1037'))
    self.assertEqual('\u101B\u103E\u1031\u1037',
                     utn11.Canonicalize('\u101B\u1037\u103E\u1031'))
    self.assertEqual('\u101B\u103E\u1031\u1037',
                     utn11.Canonicalize('\u101B\u1031\u1037\u103E'))
    self.assertEqual('\u101B\u103E\u1031\u1037',
                     utn11.Canonicalize('\u101B\u1037\u1031\u103E'))
    return


if __name__ == '__main__':
  unittest.main()
