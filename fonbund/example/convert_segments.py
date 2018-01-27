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

import sys

from fonbund import segment_to_features_converter as converter_lib


def convert_segments(converter, segments):
  status, features = converter.ToFeatures(segments)
  if not status:
    return
  # The "features" is a list of "DistinctiveFeatures" protocol buffer messages
  # (defined in fonbund/distinctive_features.proto) each corresponding to a
  # segment.
  assert len(features) == len(segments)
  return features


def format_features(distinctive_features):
  row = []
  feature_names = distinctive_features.feature_names
  for feature_list in distinctive_features.feature_list:
    for name, feature in zip(feature_names, feature_list.feature):
      value = feature.multivalued_value.string_value
      if not value:
        if feature.binary_value == converter_lib.POSITIVE:
          value = '+'
        elif feature.binary_value == converter_lib.NEGATIVE:
          value = '-'
        else:
          value = '0'
      row.append('%s:%s' % (name, value))
  return row


def main(argv):
  config_file_path = argv[1]
  segments_db_file_paths = argv[2:]
  converter = converter_lib.SegmentToFeaturesConverter()
  assert converter.OpenPaths(config_file_path, segments_db_file_paths)

  for line in sys.stdin:
    segments = line.strip().split()
    features_per_segment = convert_segments(converter, segments)
    if not features_per_segment:
      continue
    for segment, distinctive_features in zip(segments, features_per_segment):
      row = format_features(distinctive_features)
      print('%s\t%s' % (segment, ' '.join(row)))
  return


if __name__ == '__main__':
  main(sys.argv)
