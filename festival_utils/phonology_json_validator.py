#! /usr/bin/env python

# Copyright 2016, 2017 Google LLC. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the 'License');
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an 'AS IS' BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Validate phonology.json files.

Reads phonology.json file and validates the phoneme inventory.
"""

import io
import json
import sys

STDERR = io.open(2, mode='wt', encoding='utf-8', closefd=False)


def main(argv):
  with io.open(argv[1], mode='rt', encoding='utf-8') as reader:
    contents = reader.read()
  phonology = json.loads(contents)

  feature_types = {}
  features = {}
  is_valid = True

  for feature in phonology['features']:
    features.update({feature[0]: feature[1:]})

  for feat_type in phonology['feature_types']:
    for feat in feat_type[1:]:
      if not features.get(feat):
        STDERR.write('Feature %s not in %s\n' % (feat, str(features)))
        is_valid = False

    feature_types.update({feat_type[0]: feat_type[1:]})

  for phone in phonology['phones']:
    feature_list = feature_types.get(phone[1])
    phoneme = phone[0]

    expected_feature_list = len(feature_list) + 2

    if len(phone) != len(feature_list) + 2:
      STDERR.write(
          'Phoneme %s does not match its feature types, expected features %s\n'
          % (phoneme, expected_feature_list))
      is_valid = False

    for x in range(2, len(phone)):
      feature_type = feature_list[x - 2]
      feature_options = features.get(feature_type)

      if phone[x] not in feature_options:
        STDERR.write(
            'Phoneme "%s" given feature "%s" value "%s" not found in list %s\n'
            % (phoneme, feature_type, phone[x], str(feature_options)))
        is_valid = False

  sys.exit(0 if is_valid else 1)
  return


if __name__ == '__main__':
  main(sys.argv)
