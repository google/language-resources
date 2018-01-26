# coding=utf-8

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

from fonbund import helpers
from fonbund import segment_repository_config_pb2
from fonbund import segment_repository_reader


class SegmentRepositoryReaderTest(unittest.TestCase):

  def setUp(self):
    self.contents = helpers.GetResourceAsText(
        "fonbund", "testdata/tiny_panphon_example.csv")
    self.assertTrue(self.contents)
    return

  def test_read_with_empty_config(self):
    config = segment_repository_config_pb2.SegmentRepositoryConfig()
    config.field_separator = ","
    config.has_column_description = True
    repository_reader = segment_repository_reader.SegmentRepositoryReader()

    success = repository_reader.OpenFromContents(config, [self.contents])
    self.assertFalse(success)
    config.num_features = 22  # PanPhon features.
    success = repository_reader.OpenFromContents(config, [self.contents])
    self.assertTrue(success)
    self.assertTrue(repository_reader.feature_names)
    self.assertEqual("syl", repository_reader.feature_names[0])
    self.assertTrue(repository_reader.segments_to_features)

  def test_check_whitelisting_balcklisting(self):
    k_language_region = "foo-FOO"
    # Whitelist check.
    whitelist_config = segment_repository_config_pb2.SegmentRepositoryConfig()
    whitelist_config.language_list_whitelisted = True
    repository_reader = segment_repository_reader.SegmentRepositoryReader()
    self.assertTrue(repository_reader.IgnoreLanguage(whitelist_config,
                                                     k_language_region))
    whitelist_config.language_regions.append(k_language_region)
    self.assertFalse(repository_reader.IgnoreLanguage(whitelist_config,
                                                      k_language_region))
    # Blacklist check.
    blacklist_config = segment_repository_config_pb2.SegmentRepositoryConfig()
    blacklist_config.language_list_whitelisted = False
    self.assertFalse(repository_reader.IgnoreLanguage(blacklist_config,
                                                      k_language_region))
    blacklist_config.language_regions.append(k_language_region)
    self.assertTrue(repository_reader.IgnoreLanguage(blacklist_config,
                                                     k_language_region))

  def test_read_with_config_overrides(self):
    k_separator = ","
    k_segment_name = "ɫ̤ʲ"
    k_new_features = "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0"
    config = segment_repository_config_pb2.SegmentRepositoryConfig()
    config.field_separator = k_separator
    config.has_column_description = True
    config.num_features = 22
    config.extra_segment_feature_entries.append(
        k_segment_name + k_separator + k_new_features)
    repository_reader = segment_repository_reader.SegmentRepositoryReader()
    self.assertTrue(repository_reader.OpenFromContents(config, [self.contents]))
    segments_to_features = repository_reader.segments_to_features
    self.assertIn(k_segment_name, segments_to_features)
    for feature_value in segments_to_features[k_segment_name]:
      self.assertEqual("0", feature_value)


if __name__ == "__main__":
  unittest.main()
