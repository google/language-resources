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

"""Interface for reading IPA segment inventories."""

from __future__ import unicode_literals

import io

from absl import logging
from fonbund import helpers
from fonbund import segment_normalizer
from fonbund import segment_repository_config_pb2


def _RemoveIgnoredFields(ignore_ids, segment_column_id, fields):
  """Dilutes list of fields removing ones whose IDs are specified.

  The column corresponding to the segment name will always be removed.

  Args:
    ignore_ids: list of int column ids.
    segment_column_id: int column id for the segment.
    fields: list of strings to be diluted.

  Returns:
    Updates fields list.
  """
  ignore_ids_set = set(ignore_ids)
  ignore_ids_set.add(segment_column_id)
  unique_ids = list(ignore_ids_set)
  unique_ids.sort()
  for i in range(len(unique_ids) - 1, -1, -1):
    assert unique_ids[i] < len(fields)
    fields.pop(unique_ids[i])


class SegmentRepositoryReader(object):
  """Helper class for normalizing the IPA segments.

  Attributes:
    _config: SegmentNormalizerConfig сontaining configuration
    _feature_names: List of strings for feature names
    _segments_to_features:
    _ignored_column_ids:
  """

  def __init__(self):
    self._config = segment_repository_config_pb2.SegmentRepositoryConfig()
    self._feature_names = []
    self._segments_to_features = {}
    self._ignored_column_ids = []
    self._normalizer = None

  def OpenPaths(self, config_path, repository_paths):
    """Reads segment repository configuration and repository(ies) from files.

    Allows the caller to supply several repositories. The structure of all the
    given repositories has to match - for each segment, there must be the same
    number of features of the same type.

    Args:
      config_path: string, path to segment configuration text proto file.
      repository_paths: list of strings representing paths for repositories.

    Returns:
      Bool indicating success of the operation.
    """
    self._config = helpers.GetTextProto(
        config_path, segment_repository_config_pb2.SegmentRepositoryConfig())
    return self.Open(self._config, repository_paths)

  def Open(self, config, repository_paths):
    """Reads segment repositories from paths given the configuration.

    Tries not to check-fail.

    Args:
      config: SegmentRepositoryConfig proto.
      repository_contents: String buffers representing multiple repositories.

    Returns:
      Bool indicating success of the operation.
    """
    self._config = config
    repository_contents = []
    for repository_path in repository_paths:
      with io.open(repository_path, "rt", encoding="utf-8") as reader:
        repository_contents.append(reader.read())
    return self.OpenFromContents(self._config, repository_contents)

  def OpenFromContents(self, config, repository_contents):
    """Reads segment repository from contents given the configuration.

    Tries not to check-fail.

    Args:
      config: SegmentRepositoryConfig proto.
      repository_contents: String buffers representing multiple repositories.

    Returns:
      Bool indicating success of the operation.
    """
    self._config = config
    assert repository_contents

    # Cache the IDs of the columns to remove.
    self._ignored_column_ids = list(self._config.ignore_column_ids)

    # Load all the provided repositories and double-check that the column
    # descriptions match. Also merge all the segment mappings into one.
    previous_feature_names = []
    segments = {}
    num_repositories = len(repository_contents)
    for i in range(num_repositories):
      previous_feature_names = self._feature_names
      success = self._LoadRepository(
          repository_contents[i], self._feature_names, segments)
      if not success:
        logging.error("Failed to load repository %d", i)
        return False
      if (config.has_column_description and i > 0 and
          previous_feature_names != self._feature_names):
        logging.error("Repositories %d and %d have mismatching features!",
                      i - 1, i)
        return False
      self._segments_to_features.update(segments)
    # Check if we have extra repository contents defined in the configuration
    # and augment/override the current segment-to-feature mapping.
    self._normalizer = segment_normalizer.SegmentNormalizer(
        self._config.normalizer)
    for contents in self._config.extra_segment_feature_entries:
      fields = contents.split(self._config.field_separator)
      success = self._ReadRepositoryEntry(fields, self._normalizer,
                                          self._segments_to_features)
      if not success:
        return False
    return True

  @property
  def config(self):
    return self._config

  @property
  def feature_names(self):
    return self._feature_names

  @property
  def segments_to_features(self):
    return self._segments_to_features

  @property
  def normalizer(self):
    return self._normalizer

  def IgnoreLanguageShort(self, language_region):
    return self.IgnoreLanguage(self._config, language_region)

  def IgnoreLanguage(self, config, language_region):
    """Checks language for vs ignore list.

    Stores the resulting segment-to-features mapping in <segments_to_features>.
    Returns the column description as a list of field names iff
    <has_column_description> is enabled in the config.

    Args:
      config: SegmentNormalizerConfig сontaining info on ignore list and its
              logic (whitelist or blacklist).
      language_region: string representing language-region in BCP-47 format,
              e.g. en-US.

    Returns:
      Bool indicating whether language should be ignored.
    """
    language_regions = set(config.language_regions)
    language_listed = language_region in language_regions
    return config.language_list_whitelisted != language_listed

  def _LoadRepositoryFromFile(self, repository_path, feature_names,
                              segments_to_features):
    """Loads segment repository from file."""
    with io.open(repository_path, "rt", encoding="utf-8") as reader:
      contents = reader.read()
    return self._LoadRepository(contents, feature_names, segments_to_features)

  def _LoadRepository(self, repository_contents, feature_names,
                      segments_to_features):
    """Loads segment repository given the configuration.

    Stores the resulting segment-to-features mapping in <segments_to_features>.
    Returns the column description as a list of field names iff
    <has_column_description> is enabled in the config.

    Args:
      repository_contents: string buffer with repository contents.
      feature_names: list of string, updated as a result of function.
      segments_to_features: map, updated as a result of function.

    Returns:
      Bool indicating success of the operation.
    """
    field_separator = self._config.field_separator
    if len(field_separator) != 1:
      logging.error("Invalid field separator: " + field_separator)
      return False

    normalizer = segment_normalizer.SegmentNormalizer(self._config.normalizer)
    segments_to_features.clear()
    feature_names[:] = []

    line_counter = 0
    for raw_line in repository_contents.split("\n"):
      if not raw_line:
        continue
      line = raw_line.rstrip("\n").split(field_separator)
      if self._config.has_column_description and line_counter == 0:
        feature_names[:] = line
        _RemoveIgnoredFields(self._ignored_column_ids,
                             self._config.segment_column_id, feature_names)
        if not feature_names:
          logging.error("Feature names cannot be empty (raw_line: '%s', line: '%s')!",
                        raw_line, line)
          return False
        if len(feature_names) != self._config.num_features:
          logging.error("Expected %d features, got %d instead!",
                        self._config.num_features, len(feature_names))
          return False
        line_counter += 1
        continue
      success = self._ReadRepositoryEntry(line, normalizer,
                                          segments_to_features)
      if not success:
        return False
      line_counter += 1

    # Double check that we've scanned something.
    if not line_counter:
      logging.error("Invalid repository, no entries scanned!")
      return False
    if not segments_to_features:
      logging.error("No segments found!")
      return False
    logging.info("Scanned %s segments.", len(segments_to_features))
    return True

  def _ReadRepositoryEntry(self, contents, normalizer, segments_to_features):
    """Reads repository entry represented as a vector of string values.

    Args:
      contents: list of strings representing repository entry.
      normalizer: SegmentNormalizer
      segments_to_features: map, changed as a result of function.

    Returns:
      Bool indicating success of the operation.
    """
    if len(contents) <= 1:
      logging.error("Line '%s': Found fewer than two fields!", contents)
      return False
    fields = contents[:]
    # Normalize the segment name.
    segment_column_id = self._config.segment_column_id
    segment = fields[segment_column_id]
    success, segment = normalizer.MinimalNormalization(segment)
    if not success:
      logging.error("Failed to normalize segment: " + segment)
      return False
    # Drop all the columns that we don't need.
    _RemoveIgnoredFields(self._ignored_column_ids, segment_column_id, fields)
    if len(fields) != self._config.num_features:
      logging.error("Invalid number of features: Expected " +
                    str(self._config.num_features) + ", got " +
                    str(len(fields)) + " instead.")
      return False
    segments_to_features[segment] = fields
    return True
