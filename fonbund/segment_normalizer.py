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

"""Helper class for normalizing the IPA segments."""

from __future__ import unicode_literals

import unicodedata

from fonbund import segment_normalizer_config_pb2


class SegmentNormalizer(object):
  """Helper class for normalizing the IPA segments.

  Attributes:
    _config: SegmentNormalizerConfig сontaining configuration.
    _segment_rewrites: A mapping of supported rewrites
      (e.g. 'ʧʰ' -> ['tʃʰ', (l_1,...l_n)]), where each rewrite has a possibly
      empty set of language restrictions. Extracted from _config.
  """

  def __init__(self, config):
    """Initializes the normalizer given the configuration."""
    self._config = segment_normalizer_config_pb2.SegmentNormalizerConfig()
    self._config.CopyFrom(config)
    self._segment_rewrites = {}
    for rewrite in self._config.symbol_rewrites:
      restrictions = set(rewrite.restrict_to)
      assert rewrite.lhs not in self._segment_rewrites
      self._segment_rewrites[rewrite.lhs] = (rewrite.rhs, restrictions)

  def FullNormalization(self, language_region, segment):
    """Performs full normalization on a segment.

    Args:
      language_region: BCP-47 formatted language+region spec, e.g. "bn-IN".
      segment: unicode string to run normalization onto.

    Returns:
      A pair of bool (whether normalization was successful) and normalized
      unicode segment.
    """
    return self._NormalizeSegment(language_region, False, segment)

  def MinimalNormalization(self, segment):
    """Performs minimal normalization on a segment.

    This API is used for loading the segment inventories. In this mode,
    no rewrites are performed.

    Args:
      segment: unicode string to run normalization onto.

    Returns:
      A pair of bool (whether normalization was successful) and normalized
      unicode segment.
    """
    return self._NormalizeSegment("", True, segment)

  @staticmethod
  def NormalizeToNFD(segment):
    return unicodedata.normalize("NFD", segment)

  def _RemoveTieBars(self, segment):
    """Returns segment with removed tie bars."""
    symbols_to_strip = [
        # Non-spacing ties.
        u"\u0361",  # COMBINING DOUBLE INVERTED BREVE (tie bar above)
        u"\u035C",  # COMBINING DOUBLE BREVE BELOW (tie bar below)
        # Spacing ties (those should be flagged by IPA checker as invalid
        #               anyway).
        u"\u2040",  # CHARACTER TIE (tie bar above)
        u"\u203F",   # UNDERTIE (tie bar below)
    ]
    for symbol_to_strip in symbols_to_strip:
      segment = segment.replace(symbol_to_strip, "")
    return segment

  def _NormalizeSegment(self, language_region, block_rewrites, segment):
    """Base method for normalization."""
    segment = SegmentNormalizer.NormalizeToNFD(segment)
    if self._config.remove_tie_bars:
      segment = self._RemoveTieBars(segment)
    # Return early.
    if block_rewrites:
      return (True, segment)
    if segment not in self._segment_rewrites:
      return (True, segment)

    # Check that the rewrite for this language is allowed:
    #  - if the restrictions list is empty, we assume that this is a global
    #    rewrite,
    #  - otherwise, only perform the rewrite if language is in the list.
    segment_rewrite, restricted_rewrites = self._segment_rewrites[segment]
    if not restricted_rewrites or language_region in restricted_rewrites:
      segment = segment_rewrite
    return (True, segment)
