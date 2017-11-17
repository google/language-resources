# Copyright 2017 Google Inc. All Rights Reserved.
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

"""Format Hindustani parallel script data (Devanagari/Arabic) as HTML.
"""

from __future__ import unicode_literals

import re

from utils import utf8

HEADER = r'''<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta charset="utf-8">
<style type="text/css">
@import url('https://fonts.googleapis.com/css?family=Vesper+Libre');
@import url('https://fonts.googleapis.com/earlyaccess/notonastaliqurdu.css');
.arab {
    color: #006600;
    direction: rtl;
    font-family: 'Noto Nastaliq Urdu', serif;
    font-size: 32pt;
}
.deva {
    color: #000080;
    direction: ltr;
    font-family: 'Vesper Libre', 'Noto Serif Devanagari', serif;
    font-size: 32pt;
}
.table {
    border: 20pt;
    display: table;
    margin: auto;
}
.row {
    display: table-row;
}
.cell {
    display: table-cell;
    padding: 10pt 20pt;
}
</style>
<title>Hindi/Urdu</title>
</head>
<body>
<div class="table">
'''

FOOTER = r'''</div>
</body>
</html>
'''

ARAB = re.compile(r'[\u0600-\u06FF]')
DEVA = re.compile(r'[\u0900-\u097F]')


def Field2Html(field):
  arab = ARAB.search(field)
  if arab:
    assert not DEVA.search(field)
  c = 'arab' if arab else 'deva'
  return '<div class="cell %s">%s</div>' % (c, field)


def main(unused_argv):
  w = utf8.stdout
  w.write(HEADER)
  for line in utf8.stdin:
    line = line.rstrip('\n')
    if not line or line.startswith('#'):
      continue
    fields = line.split('\t')
    w.write('<div class="row">\n')
    for field in fields:
      w.write('  %s\n' % Field2Html(field))
    w.write('</div>\n')
  w.write(FOOTER)
  return


if __name__ == '__main__':
  main(None)
