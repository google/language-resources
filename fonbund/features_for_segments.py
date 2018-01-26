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

r"""Extracts the features given phonetic segments.

The features for each segment (corresponding to a the supplied combination
of phoneme segment inventory configurations) are dumped in a protocol
buffer format.

Example:
--------
In the following example, the /z ɑu d aː/ segment string will be rewritten
to the corresponding feature representations using PHOIBLE, PanPhon and
PHOIBLE Fonetikode representations:

bazel-bin/fonbund/features_for_segments \
  --databases phoible,phoible_fonetikode,panphon \
  --segments z,ɑ+u,d,aː \
  --output_features_file segments.textproto

Please note that, in the above example, the "complex" segments (such as
diphthongs) are encoded using a hard-coded FonBund component separator
"+". This will ensure proper decomposition for such cases.
"""

import sys

from absl import app
from absl import flags
from absl import logging

from fonbund import distinctive_features_pb2 as df
from fonbund import helpers
from fonbund import segment_to_features_converter as converter_lib
from fonbund import segments as segments_lib

flags.DEFINE_list(
    "databases", [],
    "List of segment inventory names, e.g. 'panphon', 'phoible' and so on.")

flags.DEFINE_list(
    "segments", [],
    "List of IPA phonetic segments.")

flags.DEFINE_string(
    "output_features_file", None,
    "Output file containing the feature representations.")

FLAGS = flags.FLAGS


def main(unused_argv):
  # Check that the segment inventories are supported.
  if not FLAGS.databases:
    logging.error("Supply --databases!")
    app.usage(shorthelp=True, exitcode=2)
  for db in FLAGS.databases:
    if db not in segments_lib.TABLES:
      logging.error("Database '%s' not recognized; possible values are %s\n",
                    db, ", ".join("'%s'" % t for t in segments_lib.TABLES))
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
    config_path, contents = segments_lib.GetConfigAndRepositoryContents(db)
    if not converter.OpenFromContents(config_path, [contents]):
      logging.error("%s: Failed to initialze converter", db)
      sys.exit(2)
    converters[db] = converter

  # For each of the databases produce the feature representation for the
  # corresponding segments.
  segments = [helpers.EnsureUnicode(segment) for segment in FLAGS.segments]
  segment_to_features = {}
  for segment in segments:
    segment_to_features[segment] = {}
  for db in databases:
    converter = converters[db]
    logging.info("%s: Converting '%s' ...", db, ",".join(segments))
    status, db_features = converter.ToFeatures(segments)
    if not status:
      logging.error("%s: Feature conversion failed.", db)
      sys.exit(2)
    if len(db_features) != len(segments):
      logging.error(("%s: Number of feature bundles should match the "
                     "number of segments"), db)
      sys.exit(2)
    # Iterate over all the segments and store the features for that segment
    # in a dictionary, where the features are database-specific.
    for segment_id in range(0, len(segments)):
      segment = segments[segment_id]
      segment_to_features[segment][db] = db_features[segment_id]

  # Collect for each segment a unified representation of all the features.
  segment_features = df.SegmentFeatures()
  for segment in segments:
    dfr = df.DistinctiveFeatureRepresentations()
    dfr.segment_name = segment
    for db in databases:
      db_features = segment_to_features[segment][db]
      dfr.representation.extend([db_features])
    segment_features.features.extend([dfr])

  # Save to the specified file or dump to stdout.
  contents = helpers.MessageToUnicodeString(segment_features)
  if FLAGS.output_features_file:
    helpers.SetTextContents(FLAGS.output_features_file, contents)
    logging.info("Wrote features to %s.", FLAGS.output_features_file)
  else:
    print(contents)

  return


if __name__ == "__main__":
  app.run(main)
