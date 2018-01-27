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

"""Utility module for interacting with segment inventory tables."""

import io

from fonbund import helpers
from fonbund import segment_repository_config_pb2 as config_pb2

TABLES = {
    'panphon': (
        'panphon',
        'data/ipa_all.csv',
        'config/segment_repository_config_panphon.textproto'),

    'phoible': (
        'clld_phoible',
        'data/phoible-segments-features.tsv',
        'config/segment_repository_config_phoible.textproto'),

    'phoible_fonetikode': (
        'phon_class_counts',
        'input/phoible_Features_Fonetikode.csv',
        'config/segment_repository_config_phoible_fonetikode.textproto'),
}


def AvailableTables():
  for table_name, (package, resource, _) in TABLES.items():
    if helpers.ResourceAvailable(package, resource):
      yield table_name
  return


def GetConfigAndRepositoryContents(table_name):
  """Returns configuration proto and repository contents."""
  package, resource, config_file = TABLES[table_name]
  # Read configuration.
  config = helpers.GetTextProtoResource(
      'fonbund', config_file, config_pb2.SegmentRepositoryConfig())
  assert config.IsInitialized()
  # Read repository contents.
  repository_contents = helpers.GetResourceAsText(package, resource)
  assert repository_contents
  return config, repository_contents


def SelectFrom(table_name):
  """Yields rows in the given segment table."""
  config, repository_contents = GetConfigAndRepositoryContents(table_name)
  with io.StringIO(repository_contents) as reader:
    for line in reader:
      line = line.rstrip('\r\n')
      row = line.split(config.field_separator)
      assert len(row) == (config.num_features + 1 +
                          len(config.ignore_column_ids))
      yield row
  return
