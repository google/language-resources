# coding=utf-8
#
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

"""Representation of IPA symbols and decomposition of strings of IPA symbols."""

from __future__ import unicode_literals

import io
import os.path
import six

from absl import logging
from google.protobuf import text_format

from fonbund import ipa_symbols_pb2
from fonbund import segment_normalizer


def _GetUnicodeFromIpaSymbolHexCode(hex_code):
  """Converts the codepoints to a Unicode string.

  Args:
    hex_code: string representing hex-code of a codepoint.

  Returns:
    string (empty if no success).
  """
  return six.unichr(int(hex_code, 16))


def _CodepointString(unicode_text):
  """Converts unicode string to string of integers in uppercase hex.

  Integers correspond to the codepoints within the string. Each integer is
  followed by a space.

  Args:
    unicode_text: unicode string

  Returns:
    string
  """
  return " ".join("%04X" % ord(c) for c in unicode_text)


class IpaSymbolsRepository(object):
  """Helper class for representing the repository of IPA symbols.

  Provides simple validation API for checking whether an invidual symbol or a
  sequence of symbols represents valid IPA.

  Attributes:
    _ipa_symbols: set of strings representing IPA symbols.
    _unofficial_ipa_symbols: mapping between unofficial (typically precomposed)
                             IPA symbols and their respective IPA strings.
    _descriptions: mapping between IPA symbols and their descriptions.
  """

  def __init__(self):
    self._ipa_symbols = set()
    self._unofficial_ipa_symbols = {}
    self._descriptions = {}

  @property
  def ipa_symbols(self):
    return self._ipa_symbols

  @property
  def descriptions(self):
    return self._descriptions

  @property
  def unofficial_ipa_symbols(self):
    return self._unofficial_ipa_symbols

  def Init(self):
    k_ipa_symbols_path = os.path.join(
        os.path.dirname(__file__), "config", "ipa_symbols.textproto")
    ipa_symbols_data = ipa_symbols_pb2.IpaSymbols()
    with io.open(k_ipa_symbols_path, "rb") as f:
      data = f.read()
      if six.PY3:
        # Appalling but apparently necessary hack to work around protobuf.
        data = data.decode('latin-1').encode('utf-8')  # NEVER TRY THIS AT HOME!
      ipa_symbols_data = text_format.Parse(data, ipa_symbols_pb2.IpaSymbols())
    return self._Init(ipa_symbols_data)

  def _Init(self, ipa_symbols):
    """Initializes the repository from the proto.

    Args:
       ipa_symbols: IpaSymbols proto instance.

    Returns:
       Boolean representing whether init was successful.
    """
    unofficial_symbols = []
    for symbol in ipa_symbols.symbol:
      # Convert the codepoints to Unicode string.
      usym = _GetUnicodeFromIpaSymbolHexCode(symbol.hex_code)
      usym = segment_normalizer.SegmentNormalizer.NormalizeToNFD(usym)
      if symbol.unofficial:
        unofficial_symbols.append((usym, symbol))
        continue
      # Update the repository.
      if usym in self._ipa_symbols:
        logging.error("Duplicate definition of " + usym + "(" +
                      symbol.hex_code + ")!")
        return False
      self._ipa_symbols.add(usym)
      self._descriptions[usym] = symbol.description
    logging.info("Scanned " + str(len(self._ipa_symbols)) + " symbols.")
    if not ipa_symbols.allow_unofficial_characters:
      return True

    # Traverse the list of ``unofficial`` IPA symbols and prepare the mapping.
    for symbol_pair in unofficial_symbols:
      unofficial, symbol = symbol_pair
      if not symbol.maps_to_hex_code:
        logging.error("``Unofficial`` symbol '" + unofficial +
                      "' doesn't map to anything")
        return False
      targets = []
      self._unofficial_ipa_symbols[unofficial] = targets
      for hex_code in symbol.maps_to_hex_code:
        usym = _GetUnicodeFromIpaSymbolHexCode(hex_code)
        if usym not in self._ipa_symbols:
          logging.error("Unknown target codepoints: " + hex_code)
          return False
        targets.append(usym)
      self._ipa_symbols.add(unofficial)
      self._descriptions[unofficial] = symbol.description
    return True

  def IsValidSymbol(self, symbol):
    """Checks if the supplied IPA symbol is valid.

    This method does not decompose the input assuming it's a single IPA symbol.

    Args:
       symbol: String.

    Returns:
       Boolean representing whether symbol is IPA valid.
    """
    return symbol in self._ipa_symbols

  def IsValidString(self, input_symbols):
    """Checks if the supplied string of IPA symbols is valid.

    This API only checks if a string can be decomposed into a sequence of legal
    IPA symbols. It does *not* check whether the resulting decomposition is
    phonologically/phonetically valid.

    Args:
       input_symbols: String.

    Returns:
       Boolean representing whether string is IPA valid.
    """
    if self.IsValidSymbol(input_symbols):
      return True
    # If symbol is not present in the repository it's possible that it consists
    # of a combination of several IPA symbols. Decompose the text and validate
    # each of the symbols.
    if not input_symbols:
      return False
    for usym in input_symbols:
      if not self.IsValidSymbol(usym):
        code_points = _CodepointString(usym)
        logging.error('Invalid IPA string "%s": Symbol "%s" (%s) not found',
                      input_symbols, usym, code_points)
        return False
    return True

  def GetDescriptionPerSymbol(self, symbols, description):
    """Fetches description for a particular sequence of IPA symbols.

    One description per symbol.

    Args:
       symbols: string.
       description: list of pairs of strings, updated by the function.

    Returns:
       False if symbol is not found, true otherwise.
    """
    description[:] = []
    for symbol in symbols:
      if symbol not in self._descriptions:
        return False
      description.append((symbol, self._descriptions[symbol]))
    if not description:
      return False
    return True

  def GetDescription(self, symbols, description):
    """Fetches description for a particular sequence of IPA symbols.

    One description per symbol.

    Args:
       symbols: string.
       description: list of strings, updated by the function.

    Returns:
       False if symbol is not found, true otherwise.
    """
    symbol_descriptions = []
    if not self.GetDescriptionPerSymbol(symbols, symbol_descriptions):
      return False
    description[:] = [elem[1] for elem in symbol_descriptions]
    return True
