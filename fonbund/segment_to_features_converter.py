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

"""Interface for converting segments to the articulatory features."""

from __future__ import unicode_literals

from absl import logging

from fonbund import distinctive_features_pb2 as df
from fonbund import ipa_symbols_repository as ipa
from fonbund import segment_repository_reader

# Separator character for marking constituents of complex segments, such as
# diphthongs.
COMPLEX_SEGMENT_SEPARATOR = "+"

# Forward enum constants for convenient client-code usage.
NOT_APPLICABLE = df.DistinctiveFeature.NOT_APPLICABLE
POSITIVE = df.DistinctiveFeature.POSITIVE
NEGATIVE = df.DistinctiveFeature.NEGATIVE


def _BinaryFeatureToValue(value):
  """Converts string value of a binary distinctive feature to an value.

  Args:
     value: String value.
  Returns:
     Boolean status and distinctive feature value tuple.
  """
  feature = df.DistinctiveFeature()
  if value == "0":
    feature.binary_value = df.DistinctiveFeature.NOT_APPLICABLE
  elif value == "+":
    feature.binary_value = df.DistinctiveFeature.POSITIVE
  elif value == "-":
    feature.binary_value = df.DistinctiveFeature.NEGATIVE
  else:
    # Complex segments sometimes are represented with multivalued features,
    # e.g. in PHOIBLE, where for diphthongs/triphthongs/etc. the feature may
    # take multiple values, e.g. "+,-". Because we require complext segments
    # to be decomposed using the COMPLEX_SEGMENT_SEPARATOR, we may need
    # to implement a more complex logic rather than erroring here.
    logging.error("Invalid binary string feature value: '%s'", value)
    return False, None
  return True, feature


def _MultivaluedFeatureToValue(config, value):
  """Converts string value of a binary distinctive feature to an value.

  Args:
     config: Segment repository configuration.
     value: String value.
  Returns:
     Boolean status and distinctive feature value.
  """
  feature = df.DistinctiveFeature()
  fv = feature.multivalued_value
  if value:
    fv.string_value = value
  elif config.multivalued_feature_default_value:
    fv.string_value = config.multivalued_feature_default_value
  else:
    logging.error("Empty multivalued string feature value!")
    return False, None
  return True, feature


class SegmentToFeaturesConverter(object):
  """A helper class for rewriting segments as feature bundles."""

  def __init__(self):
    self._reader = segment_repository_reader.SegmentRepositoryReader()
    self._ipa_symbols = ipa.IpaSymbolsRepository()

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
    if not self._ipa_symbols.Init():
      logging.error("Failed to initialize IPA symbols repository")
      return False
    return self._reader.OpenPaths(config_path, repository_paths)

  def OpenFromContents(self, config, repository_contents):
    """Reads segment repository from contents given the configuration.

    Tries not to check-fail.

    Args:
      config: SegmentRepositoryConfig proto.
      repository_contents: list of strings representing repository contents.

    Returns:
      Bool indicating success of the operation.
    """
    if not self._ipa_symbols.Init():
      logging.error("Failed to initialize IPA symbols repository")
      return False
    return self._reader.OpenFromContents(config, repository_contents)

  def _DecomposeSegment(self, segment):
    """Decomposes segment into constituent parts (if possible).

    Args:
       segment: Segment to be decomposed.

    Returns:
       List containing segment parts.
    """
    return segment.split(COMPLEX_SEGMENT_SEPARATOR)

  def ToFeatures(self, segments):
    """Converts each segment in a <segments> list to the corresponding features.

    Args:
       segments: A list of strings representing segments.
    Returns:
       Boolean status and a list of distinctive features protos.
    """
    config = self._reader.config
    repo_name = config.name
    mvf_default = config.multivalued_feature_default_value
    normalizer = self._reader.normalizer
    segments_to_features = self._reader.segments_to_features
    result = []
    for segment in segments:
      # Perform very minimal normalization.
      status, segment = normalizer.MinimalNormalization(segment)
      if not status:
        logging.error("Minimal normalization failed for %s", segment)
        return False, None
      # Fill the feature list header.
      feature_list = df.DistinctiveFeatures()
      feature_list.source_name = config.name
      if mvf_default:
        feature_list.multivalued_feature_default_value = mvf_default
      feature_list.feature_names.extend(self._reader.feature_names)
      # Decompose incoming segment into components and look up their individual
      # features.
      segment_components = self._DecomposeSegment(segment)
      for segment_component in segment_components:
        # Normalize the segment component and make sure it represents sane IPA.
        status, segment_component = normalizer.FullNormalization(
            "", segment_component)
        if not status:
          logging.error("Full normalization failed for %s", segment_component)
          return False, None
        if not self._ipa_symbols.IsValidString(segment_component):
          logging.error("Component %s is not valid IPA", segment_component)
          return False, None
        # Lookup individual string values for the component.
        if segment_component not in segments_to_features:
          logging.error(("%s: Segment '%s' not found! Please check if this "
                         "happens to be a complex segment, in which case "
                         "please decompose it using the '%s' separator."),
                        repo_name, segment_component, COMPLEX_SEGMENT_SEPARATOR)
          return False, None
        features = feature_list.feature_list.add()
        feature_strings = segments_to_features[segment_component]
        # Fill in the proto.
        for value_string in feature_strings:
          if not config.multivalued_features:
            status, feature = _BinaryFeatureToValue(value_string)
          else:
            status, feature = _MultivaluedFeatureToValue(config, value_string)
          if not status:
            logging.error("%s: Failed to convert '%s' to a feature value!",
                          repo_name, value_string)
            return False, None
          features.feature.extend([feature])

      # Update the result with the feature list filled for the given segment.
      result.append(feature_list)

    return True, result
