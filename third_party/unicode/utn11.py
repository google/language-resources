"""Myanmar diacritic storage order according to Unicode Technical Note #11.
"""

from __future__ import unicode_literals

import re

# BEGIN QUOTE
#
# The code after this comment and up to END QUOTE is taken from Unicode
# Technical Note #11, version 4, page 15, available from
# http://unicode.org/notes/tn11/ .
#
# Copyright (C) 2004-2012 Martin Hosken and Unicode, Inc. All Rights
# Reserved. The Unicode Consortium, and Martin Hosken make no expressed or
# implied warranty of any kind, and assume no liability for errors or
# omissions. No liability is assumed for incidental and consequential damages
# in connection with or arising out of the use of the information or programs
# contained or accompanying this technical note. The Unicode (Terms of Use)[1]
# apply.
#
# [1] For the Unicode Terms of Use, see the file LICENSE.
#
# Modifications:
#
# * Changed ur'' strings to r'' strings in conjunction with
#
#     from __future__ import unicode_literals
#
#   above. This is to ensure compatibility with Python 2 and 3.
#
# * Changed capturing groups '(...)' in regular expressions to non-capturing
#   groups '(?:...)'.

def e(x) :
    '''Expand $var in a string'''
    return re.sub(r'\$([a-z]+)', lambda m: globals()[m.group(1)], x)

# Basic character classes
kinzi = e(r'(?:[\u1004\u101B\u105A]\u103A\u1039)')
cons = e(r'[\u1000-\u102A\u103F\u1041-\u1049\u104E\u1050-\u1055\u105A-\u105D\u1061\u1065\u1066\u106E\u1070\u1075-\u1081\u108E\u109F\uA9E0-\uA9E4\uA9E7-\uA9EF\uA9FA-\uA9FE\uAA60-\uAA6F\uAA71-\uAA76\uAA7A\uAA7E\uAA7F]')
stack = e(r'(?:\u1039[\u1000-\u101C\u101E\u1020\u1021\u1050\u1051\u105A-\u105D])')
asat = e(r'\u103A')
my = e(r'[\u103B\u105E\u105F]')
mr = e(r'\u103C')
mw = e(r'[\u103D\u1082]')
mwa = e(r'\u103D')
mwb = e(r'\u1082')
mh = e(r'[\u103E\u1060]')
mha = e(r'\u103E')
mhb = e(r'\u1060')
uvowelna = e(r'[\u102D\u102E\u1033-\u1035\u1071-\u1074\u1085\u109D\uA9E5]')
uvowel = e(r'[\u102D\u102E\u1032-\u1036\u1071-\u1074\u1085\u109D\uA9E5]')
lvowel = e(r'[\u102F\u1030\u1058\u1059]')
shan = e(r'\u1086')
avowel = e(r'[\u102B\u102C\u1056\u1057\u1062\u1063\u1067\u1068\u1083]')
anusvara = e(r'[\u1036\u1032]')
pwo = e(r'[\u1064\u1069-\u106D]')
ldot = e(r'\u1037')
visarga = e(r'[\u1038\u1087-\u108D\u108F\u109A-\u109C\uAA7B-\uAA7D]')
redup = e(r'[\uA9E6\uAA70]')
symbol = e(r'[\u104A-\u104D\u104F\u109E\u109F\uAA77-\uAA79]')
digit = e(r'[\u1090-\u1099\uA9F0-\uA9F9]')

# Complex Expansions
tail = e(r'(?:$visarga?$redup?)')
finals = e(r'(?:$ldot?$tail)')
avowels = e(r'''(?:                           # handle -a and all that follows it
    (?:$avowel(?:(?:$asat?$anusvara$finals)|$ldot?$asat?$tail))   # normal a vowel
    |(?:\u102C\u103E\u103A$visarga?$redup?)                       # mon contraction
    |(?:(?:$avowel$anusvara?)?$pwo$ldot?$asat?$tail))''')         # pwo tone
ending = e(r'(?:$tail|$avowels)')
uvowels = e(r'''(?:                           # upper vowel sequences
    (?:(?:$uvowel\u1062$shan?|$uvowelna$lvowel$shan?)(?:$anusvara?$finals|$avowels))
    |(?:$anusvara$ldot?$ending)                                   # anusvara acting as vowel
    |(?:$uvowelna$shan?$anusvara?$finals)                         # anusvara always before ldot
    |(?:$uvowelna$shan?$ldot?$avowels))''')                       # ldot occuring early
lvowels = e(r'(?:(?:$lvowel|\u1062)$shan?$anusvara?(?:$finals|$avowels))')
nuvowels = e(r'''(?:$lvowels|(?:(?:$shan?|$shan$ldot)$ending))''')
asatmed = e(r'(?:(?:$my$mr?|$mr)$mw?$mh?|$mwa$mha?|$mw?$mhb)')
asats = e(r'''(?:                             # handle medials and asats
    (?:$asat$asatmed?)                                            # initial asat
    |(?:(?:$mwb$mha?|$mha)$asat)                                  # mon contractions
    |(?:$my$mr?$mw?$mh?|$mr$mw?$mh?|$mw$mh?|$mh))''')             # no asat + medial (non-empty)
evowels = e(r'(?:\u1031{0,2}|\u1084)')
myregex = e(r'''(?:$kinzi?$cons$stack{0,2}(?:                 # syllable start
        (?:$asats$evowels(?:$uvowels|$nuvowels))                  # no ldot directly after asats
        |(?:$asat?$asatmed$evowels$ldot$ending)                   # asat + medials + ldot
        |(?:$evowels(?:$uvowels|$nuvowels|$ldot$ending))          # empty, no medials
        |(?:$ldot$asat$ending))                                   # ldot + asat
    |$symbol|$digit|(?:\u104B\u1038))''')                     # other 'syllables'

# END QUOTE

CLUSTER = re.compile('%s$' % myregex, re.VERBOSE)
CLUSTERS = re.compile('%s*$' % myregex, re.VERBOSE)
