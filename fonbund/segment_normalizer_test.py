# coding=utf-8
#
# Copyright 2018 Google LLC. All Rights Reserved.
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

from __future__ import unicode_literals

import unittest

from fonbund import segment_normalizer
from fonbund import segment_normalizer_config_pb2

class TestSegmentNormalizer(unittest.TestCase):

  def test_check_ascii(self):
    config = segment_normalizer_config_pb2.SegmentNormalizerConfig()
    normalizer = segment_normalizer.SegmentNormalizer(config)
    segment = "ascii"
    success, norm_segment = normalizer.MinimalNormalization(segment)
    self.assertTrue(success)
    self.assertEqual("ascii", norm_segment)

  def test_check_tie_bars(self):
    config = segment_normalizer_config_pb2.SegmentNormalizerConfig()
    config.remove_tie_bars = True
    normalizer = segment_normalizer.SegmentNormalizer(config)
    segment = "ʈ͡ʂʷˤʰ"
    success, norm_segment = normalizer.MinimalNormalization(segment)
    self.assertTrue(success)
    self.assertEqual("ʈʂʷˤʰ", norm_segment)
    segment = "k͜p"
    success, norm_segment = normalizer.MinimalNormalization(segment)
    self.assertTrue(success)
    self.assertEqual("kp", norm_segment)

  def test_check_default_rewrites(self):
    config = segment_normalizer_config_pb2.SegmentNormalizerConfig()
    rewrite = config.symbol_rewrites.add()
    rewrite.lhs = "ɓⁿ"
    rewrite.rhs = "bⁿ"
    normalizer = segment_normalizer.SegmentNormalizer(config)
    segment = "ɓⁿ"
    success, norm_segment = normalizer.FullNormalization("", segment)
    self.assertTrue(success)
    self.assertEqual("bⁿ", norm_segment)
    success, norm_segment = normalizer.FullNormalization("en-US", segment)
    self.assertTrue(success)
    self.assertEqual("bⁿ", norm_segment)

  def test_check_restricted_rewrites(self):
    config = segment_normalizer_config_pb2.SegmentNormalizerConfig()
    rewrite = config.symbol_rewrites.add()
    rewrite.lhs = "ɳɖɽ"
    rewrite.rhs = "ɳɖr"
    k_allowed_language_region = "foo-FOO"
    rewrite.restrict_to.append(k_allowed_language_region)
    normalizer = segment_normalizer.SegmentNormalizer(config)
    segment = "ɳɖɽ"
    success, norm_segment = normalizer.FullNormalization("en-US", segment)
    self.assertTrue(success)
    self.assertEqual("ɳɖɽ", norm_segment)
    success, norm_segment = normalizer.FullNormalization(
        k_allowed_language_region, segment)
    self.assertTrue(success)
    self.assertEqual("ɳɖr", norm_segment)


if __name__ == "__main__":
  unittest.main()
