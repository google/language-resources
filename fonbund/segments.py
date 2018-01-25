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
import pkgutil

TABLES = {
    'panphon': ('PanPhon', 'panphon/data/ipa_all.csv', ',', 23),
    'phoible': ('clld_phoible', 'data/phoible-segments-features.tsv', '\t', 38),
    'phoible_fonetikode': {'phon_class_counts', 'input/phoible_Features_Fonetikode.csv', '\t', 16},
}


def SelectFrom(table_name):
  """Yields rows in the given segment table."""
  package, resource, delimiter, columns = TABLES[table_name]
  data = pkgutil.get_data(package, resource)
  assert data
  with io.TextIOWrapper(io.BytesIO(data), encoding='utf-8') as reader:
    for line in reader:
      line = line.rstrip('\n')
      row = line.split(delimiter)
      assert len(row) == columns
      yield row
  return
