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

"""Writes IPA symbols as UTF-8 text to stdout.
"""

import io
import pkgutil

from absl import app
from fonbund import ipa_symbols_pb2
import six
from google.protobuf import text_format

STDOUT = io.open(1, mode='wt', encoding='utf-8', closefd=False)


def IpaSymbols():
  data = pkgutil.get_data('fonbund.show_ipa_symbols', 'ipa_symbols.textproto')
  if six.PY3:
    # Appalling but apparently necessary hack to work around protobuf.
    data = data.decode('latin-1').encode('utf-8')  # NEVER TRY THIS AT HOME!
  return text_format.Parse(data, ipa_symbols_pb2.IpaSymbols())


def main(unused_argv):
  symbols = IpaSymbols()
  for ipa_symbol in symbols.symbol:
    symbol = ipa_symbol.symbol
    assert len(symbol) == 1
    assert ord(symbol) == ipa_symbol.ucs_code
    STDOUT.write('%s\t%s\n' % (symbol, ipa_symbol.description))
  return


if __name__ == '__main__':
  app.run(main)
