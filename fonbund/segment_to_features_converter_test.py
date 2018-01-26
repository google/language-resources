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

"""Simple tests for segment to features converter."""

from __future__ import unicode_literals

import os
import unittest

from fonbund import distinctive_features_pb2 as df
from fonbund import segment_to_features_converter as converter_lib


class SegmentToFeaturesConverterTest(unittest.TestCase):

  def test_simple_panphon_features(self):
    """Tests simple segments."""
    panphon_segments_path = os.path.join(
        os.path.dirname(__file__), "testdata", "tiny_panphon_example.csv")
    panphon_config_path = os.path.join(
        os.path.dirname(__file__), "config",
        "segment_repository_config_panphon.textproto")
    converter = converter_lib.SegmentToFeaturesConverter()
    self.assertTrue(converter.OpenPaths(panphon_config_path,
                                        [panphon_segments_path]))
    segments = ["ɡᶣ", "ɫ̤ʲ", "ɫ̤ˠ"]
    status, features = converter.ToFeatures(segments)
    self.assertTrue(status)
    num_features = 22
    feature_names = [
        u'syl', u'son', u'cons', u'cont', u'delrel', u'lat', u'nas', u'strid',
        u'voi', u'sg', u'cg', u'ant', u'cor', u'distr', u'lab', u'hi', u'lo',
        u'back', u'round', u'velaric', u'tense', u'long',
    ]
    self.assertEqual(3, len(features))
    self.assertEqual("PanPhon", features[0].source_name)
    self.assertListEqual(feature_names, list(features[0].feature_names))
    self.assertEqual(1, len(features[0].feature_list))
    self.assertEqual(num_features, len(features[0].feature_list[0].feature))
    self.assertEqual(df.DistinctiveFeature.NEGATIVE,
                     features[0].feature_list[0].feature[1].binary_value)  # Sonorant.
    self.assertEqual("PanPhon", features[1].source_name)
    self.assertListEqual(feature_names, list(features[1].feature_names))
    self.assertEqual(1, len(features[1].feature_list))
    self.assertEqual(num_features, len(features[1].feature_list[0].feature))
    self.assertEqual(df.DistinctiveFeature.POSITIVE,
                     features[1].feature_list[0].feature[1].binary_value)  # Sonorant.
    self.assertEqual("PanPhon", features[2].source_name)
    self.assertListEqual(feature_names, list(features[2].feature_names))
    self.assertEqual(1, len(features[2].feature_list))
    self.assertEqual(num_features, len(features[2].feature_list[0].feature))
    self.assertEqual(df.DistinctiveFeature.POSITIVE,
                     features[2].feature_list[0].feature[1].binary_value)  # Sonorant.

  def test_bad_segment(self):
    """Tests bad segments."""
    panphon_segments_path = os.path.join(
        os.path.dirname(__file__), "testdata", "tiny_panphon_example.csv")
    panphon_config_path = os.path.join(
        os.path.dirname(__file__), "config",
        "segment_repository_config_panphon.textproto")
    converter = converter_lib.SegmentToFeaturesConverter()
    self.assertTrue(converter.OpenPaths(panphon_config_path,
                                        [panphon_segments_path]))
    bad_segments = ["ሜኽ"]
    status, _ = converter.ToFeatures(bad_segments)
    self.assertFalse(status)


  def test_complex_segments(self):
    """Tests complex segments."""
    panphon_segments_path = os.path.join(
        os.path.dirname(__file__), "testdata", "tiny_panphon_example.csv")
    panphon_config_path = os.path.join(
        os.path.dirname(__file__), "config",
        "segment_repository_config_panphon.textproto")
    converter = converter_lib.SegmentToFeaturesConverter()
    self.assertTrue(converter.OpenPaths(panphon_config_path,
                                        [panphon_segments_path]))

    # Complex segments should be decomposed using a supplied delimiter.
    complex_segments = ["ɡᶣɫ̤ʲɫ̤ˠ"]
    status, _ = converter.ToFeatures(complex_segments)
    self.assertFalse(status)  # Fails if not delimited.
    complex_segments[0] = converter_lib.COMPLEX_SEGMENT_SEPARATOR.join(
        ["ɡᶣ", "ɫ̤ʲ", "ɫ̤ˠ"])
    status, _ = converter.ToFeatures(complex_segments)
    self.assertTrue(status)


if __name__ == "__main__":
  unittest.main()
