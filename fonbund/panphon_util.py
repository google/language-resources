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

"""Utility module for interacting with PanPhon."""

import io
import pkgutil

import unicodecsv


def IpaSegments():
  """Yields rows in the PanPhon segment inventory table."""
  ipa_all = pkgutil.get_data('PanPhon', 'panphon/data/ipa_all.csv')
  byte_stream = io.BytesIO(ipa_all)
  parser = unicodecsv.reader(byte_stream, encoding='utf-8', delimiter=',')
  for row in parser:
    assert len(row) == 23
    yield row
  return
