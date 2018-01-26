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

"""Extracts the features given phonetic segments.

The features for each segment (corresponding to a the supplied combination
of phoneme segment inventory configurations) are dumped in a protocol
buffer format.
"""

import io
import sys

from absl import app
from absl import flags
from absl import logging

from fonbund import distinctive_features_pb2 as df
from fonbund import segment_to_features_converter as converter_lib
from fonbund import segments

flags.DEFINE_list(
    "databases", [],
    "List of segment inventory names, e.g. 'panphon', 'phoible' and so on.")

flags.DEFINE_list(
    "segments", [],
    "List of IPA phonetic segments.")

FLAGS = flags.FLAGS


def main(unused_argv):
  # Check that the segment inventories are supported.
  if not FLAGS.databases:
    logging.error("Supply --databases!")
    app.usage(shorthelp=True, exitcode=2)
  for db in FLAGS.databases:
    if db not in segments.TABLES:
      logging.error("Database '%s' not recognized; possible values are %s\n",
                    db, ", ".join("'%s'" % t for t in segments.TABLES))
      sys.exit(2)
  if not FLAGS.segments:
    logging.error("Supply --segments!")
    app.usage(shorthelp=True, exitcode=2)

  # Prepare segment to articulatory feature converters for all the databases.
  databases = list(set(FLAGS.databases))
  databases.sort()
  converters = {}
  for db in databases:
    converter = converter_lib.SegmentToFeaturesConverter()
    config_path, contents = segments.GetConfigAndRepositoryContents(db)
    if not converter.OpenFromContents(config_path, [contents]):
      logging.error("Failed to initialze converter for '%s'", db)
      sys.exit(2)
    converters[db] = converter

  # TODO: Iterate over segments. For each segment produce a proto representing the
  # segment features in all the requested configurations.
  return


if __name__ == "__main__":
  app.run(main)
