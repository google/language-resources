#! /usr/bin/python
# -*- coding: utf-8 -*-
#
# Copyright 2016 Google Inc. All Rights Reserved.
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

import codecs
import json
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)


def main(args):
  file_name = args[1]
  contents = codecs.open(file_name, 'r', 'utf-8').read()
  phonology = json.loads(contents)

  feature_types = {}
  features = {}
  is_valid = True

  for feature in phonology['features']:
    features.update({feature[0]: feature[1:]})

  for feat_type in phonology['feature_types']:
    for feat in feat_type[1:]:
      if not features.get(feat):
        STDERR.write('Feature %s not in %s\n' %(feat, str(features)))
        is_valid = False

    feature_types.update({feat_type[0]: feat_type[1:]})

  for phone in phonology['phones']:
    feature_list = feature_types.get(phone[1])
    phoneme = phone[0]

    expected_feature_list = len(feature_list) + 2

    if not len(phone) == len(feature_list) + 2:
      STDERR.write("Phoneme %s dose not match its feature types, expected features %s", phoneme, expected_feature_list)
      is_valid = False

    for x in xrange(2, len(phone)):
      feature_type = feature_list[x - 2]
      feature_options = features.get(feature_type)

      if phone[x] not in feature_options:
        STDERR.write('Phoneme "%s" given feature "%s" value "%s" not found in list %s\n' %(phoneme, feature_type, phone[x], str(feature_options)))
        is_valid = False

  if not is_valid:
    sys.exit(0)

if __name__ == '__main__':
  main(sys.argv)
