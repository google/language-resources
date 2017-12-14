#! /usr/bin/env python
#
# Copyright 2016 Google Inc. All Rights Reserved.
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

"""Script used for creating a comparsion table between Zawgyi and Unicode.

Reads two-column tab-separated values from stdin and writes a LaTeX document to
stdout. Column 1 of the input is expected to be in Zawgyi encoding; column 2 is
expected to be in Unicode 5.1 encoding. The resulting document can be compiled
with LuaLaTeX from a recent version of TeX Live (tested with tl-20160609).

The output document requires the following TrueType fonts:

* Noto Sans, which can be downloaded from
  https://www.google.com/get/noto/

* Padauk, which can be downloaded from
  http://scripts.sil.org/cms/scripts/page.php?item_id=Padauk
  (tested with version 2.8)

* Zawgyi-One, which was originally distributed by
  http://web.archive.org/web/20120628203851/http://www.zawgyi.net/
  and which can be downloaded from various websites, including e.g.
  http://www.rfa.org/burmese/help/ZawgyiOne.ttf
  (md5sum: e5e2acb2d3bdf1d128355125e41f1964)

"""

import codecs
import sys

STDIN = codecs.getreader('utf-8')(sys.stdin)
STDOUT = codecs.getwriter('utf-8')(sys.stdout)
STDERR = codecs.getwriter('utf-8')(sys.stderr)

DOCUMENT_HEADER = r'''\documentclass{article}
\usepackage{geometry}
\geometry{paperwidth=595bp,paperheight=792bp,hmargin=72bp,vmargin=90bp}
\usepackage{fontspec}
\setmainfont{Noto Sans}
\newfontface\unicode{Padauk-book}[Path=fonts/,Scale=1.16,Script=Myanmar]
%%\newfontface\unicode{mm3}[Path=fonts/,Scale=1.05,Script=Myanmar]
\newfontface\zawgyi{ZawgyiOne}[Path=fonts/]
\usepackage{longtable}
\usepackage[table]{xcolor}
\definecolor{light-gray}{gray}{0.95}
\renewcommand{\arraystretch}{1.5}
\setlength{\tabcolsep}{24bp}
\begin{document}
\begin{center}
\large
\rowcolors{1}{light-gray}{white}
\begin{longtable}{r l l}
\textbf{Line} & \textbf{Zawgyi}  & \textbf{Codepoints} \\*
              & \textbf{Unicode} & \\[16bp]
\endhead
'''

TABLE_ITEM = r'''
%s & {\zawgyi %s}  & {\small %s} \\*
%s & {\unicode %s} & {\small %s} \\[12bp]
'''

DOCUMENT_FOOTER = r'''
\end{longtable}
\end{center}
\end{document}
'''


def main(unused_argv):
  STDOUT.write(DOCUMENT_HEADER)
  n = 0
  for line in STDIN:
    line = line.rstrip('\n')
    fields = line.split('\t')
    assert len(fields) == 2
    z, u = fields
    n += 1
    STDOUT.write(TABLE_ITEM %
                 ('%4d' % n, z, ' '.join('%04X' % ord(c) for c in z),
                  ' ' * 4,   u, ' '.join('%04X' % ord(c) for c in u)))
  STDOUT.write(DOCUMENT_FOOTER)
  return


if __name__ == '__main__':
  main(sys.argv)
