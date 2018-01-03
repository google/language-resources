# Pali and Sanskrit transliteration

This directory contains ICU transform rules for working with Pali and Sanskrit
text in various scripts. When working with Pali/Sanskrit, it is convenient to
use the following short character names instead of the official Unicode
character names:

symbolic  | Thai     | Laoo     | Lana     | Mymr     | Khmr
----------|----------|----------|----------|----------|----------
ka        | ก 0E01   | ກ 0E81   | ᨠ 1A20   | က 1000   | ក 1780
kha       | ข 0E02   | ຂ 0E82   | ᨡ 1A21   | ခ 1001   | ខ 1781
high_kxa  | ฃ 0E03   | ▧ 0E83   | ᨢ 1A22   |          |
ga        | ค 0E04   | ຄ 0E84   | ᨣ 1A23   | ဂ 1002   | គ 1782
low_kxa   | ฅ 0E05   | ▧ 0E85   | ᨤ 1A24   |          |
gha       | ฆ 0E06   | ຆ 0E86   | ᨥ 1A25   | ဃ 1003   | ឃ 1783
nga       | ง 0E07   | ງ 0E87   | ᨦ 1A26   | င 1004   | ង 1784
ca        | จ 0E08   | ຈ 0E88   | ᨧ 1A27   | စ 1005   | ច 1785
cha       | ฉ 0E09   | ຉ 0E89   | ᨨ 1A28   | ဆ 1006   | ឆ 1786
ja        | ช 0E0A   | ຊ 0E8A   | ᨩ 1A29   | ဇ 1007   | ជ 1787
low_sa    | ซ 0E0B   | ▧ 0E8B   | ᨪ 1A2A   |          |
jha       | ฌ 0E0C   | ຌ 0E8C   | ᨫ 1A2B   | ဈ 1008   | ឈ 1788
nya       | ญ 0E0D   | ຎ **0E8E** | ᨬ 1A2C   | ဉ 1009   | ញ 1789
nnya      |          |          |          | ည 100A   |
high_dda  | ฎ 0E0E   |          |          |          |
tta       | ฏ 0E0F   | ຏ 0E8F   | ᨭ 1A2D   | ဋ 100B   | ដ 178A
ttha      | ฐ 0E10   | ຐ 0E90   | ᨮ 1A2E   | ဌ 100C   | ឋ 178B
dda       | ฑ 0E11   | ຑ 0E91   | ᨯ 1A2F   | ဍ 100D   | ឌ 178C
ddha      | ฒ 0E12   | ຒ 0E92   | ᨰ 1A30   | ဎ 100E   | ឍ 178D
nna       | ณ 0E13   | ຓ 0E93   | ᨱ 1A31   | ဏ 100F   | ណ 178E
high_da   | ด 0E14   | ດ 0E94   |          |          |
ta        | ต 0E15   | ຕ 0E95   | ᨲ 1A32   | တ 1010   | ត 178F
tha       | ถ 0E16   | ຖ 0E96   | ᨳ 1A33   | ထ 1011   | ថ 1790
da        | ท 0E17   | ທ 0E97   | ᨴ 1A34   | ဒ 1012   | ទ 1791
dha       | ธ 0E18   | ຘ 0E98   | ᨵ 1A35   | ဓ 1013   | ធ 1792
na        | น 0E19   | ນ 0E99   | ᨶ 1A36   | န 1014   | ន 1793
high_ba   | บ 0E1A   | ບ 0E9A   | ᨷ 1A37   |          |
pa        | ป 0E1B   | ປ 0E9B   | ᨸ 1A38   | ပ 1015   | ប 1794
pha       | ผ 0E1C   | ຜ 0E9C   | ᨹ 1A39   | ဖ 1016   | ផ 1795
high_fa   | ฝ 0E1D   | ຝ 0E9D   | ᨺ 1A3A   |          |
ba        | พ 0E1E   | ພ 0E9E   | ᨻ 1A3B   | ဗ 1017   | ព 1796
low_fa    | ฟ 0E1F   | ຟ 0E9F   | ᨼ 1A3C   |          |
bha       | ภ 0E20   | ຠ 0EA0   | ᨽ 1A3D   | ဘ 1018   | ភ 1797
ma        | ม 0E21   | ມ 0EA1   | ᨾ 1A3E   | မ 1019   | ម 1798
ya        | ย 0E22   | ຍ 0E8D   | ᨿ 1A3F   | ယ 101A   | យ 1799
high_ya   |          | ຢ **0EA2** | ᩀ 1A40   |          |
ra        | ร 0E23   | ຣ 0EA3   | ᩁ 1A41   | ရ 101B   | រ 179A
ri        | ฤ 0E24   | ▧ 0EA4   | ᩂ 1A42   | ၒ 1052   | ឫ 17AB
la        | ล 0E25   | ລ 0EA5   | ᩃ 1A43   | လ 101C   | ល 179B
li        | ฦ 0E26   | ▧ 0EA6   | ᩄ 1A44   | ၔ 1054   | ឭ 17AD
va        | ว 0E27   | ວ 0EA7   | ᩅ 1A45   | ဝ 101D   | វ 179C
sha       | ศ 0E28   | ຨ 0EA8   | ᩆ 1A46   | ၐ 1050   | ឝ 179D
ssa       | ษ 0E29   | ຩ 0EA9   | ᩇ 1A47   | ၑ 1051   | ឞ 179E
sa        | ส 0E2A   | ສ 0EAA   | ᩈ 1A48   | သ 101E   | ស 179F
ha        | ห 0E2B   | ຫ 0EAB   | ᩉ 1A49   | ဟ 101F   | ហ 17A0
lla       | ฬ 0E2C   | ຬ 0EAC   | ᩊ 1A4A   | ဠ 1020   | ឡ 17A1
a         | อ 0E2D   | ອ 0EAD   | ᩋ 1A4B   | အ 1021   | អ 17A2
low_ha    | ฮ 0E2E   | ຮ 0EAE   | ᩌ 1A4C   |
i         |          |          | ᩍ 1A4D   | ဣ 1023   | ឥ 17A5
ii        |          |          | ᩎ 1A4E   | ဤ 1024   | ឦ 17A6
u         |          |          | ᩏ 1A4F   | ဥ 1025   | ឧ 17A7
uu        |          |          | ᩐ 1A50   | ဦ 1026   | ឩ 17A9
e         |          |          | ᩑ 1A51   | ဧ 1027   | ឯ 17AF
o1        |          |          |          |          | ឱ 17B1
o         |          |          | ᩒ 1A52   | ဩ 1029   | ឲ 17B2
ellipsis  | ฯ 0E2F   | ຯ 0EAF   | ᪪ 1AAA   |          | ។ 17D4
visarga   | กะ 0E30  | ກະ 0EB0  | ᨠᩡ 1A61  | ကး 1038  | កះ 17C7
-aa       | กา 0E32  | ກາ 0EB2  | ᨠᩣ 1A63  | ကာ 102C  | កា 17B6
-i        | กิ 0E34  | ກິ 0EB4  | ᨠᩥ 1A65  | ကိ 102D  | កិ 17B7
-ii       | กี 0E35  | ກີ 0EB5  | ᨠᩦ 1A66  | ကီ 102E  | កី 17B8
-u        | กุ 0E38  | ກຸ 0EB8  | ᨠᩩ 1A69  | ကု 102F  | កុ 17BB
-uu       | กู 0E39  | ກູ 0EB9  | ᨠᩪ 1A6A  | ကူ 1030  | កូ 17BC
joiner    |          |          | ᨠ᩠ᨠ 1A60 | က္က 1039 | ក្ក 17D2
virama    | กฺ 0E3A  | ກ຺ 0EBA  | ᨠ᩺ 1A7A  | က် 103A  | ក៑ 17D1
e- / -e   | เก 0E40  | ເກ 0EC0  | ᨠᩮ 1A6E  | ကေ       | កេ 17C1
o- / -o   | โก 0E42  | ໂກ 0EC2  | ᨠᩮᩣ      | ကော      | កោ 17C4
anusvara  | กํ 0E4D  | ກໍ 0ECD  | ᨠᩴ 1A74  | ကံ 1036  | កំ 17C6
section   | ๚ 0E5A   | ▧ 0E5B   | ᪫ 1AAB   | ။ 104b   | ៕ 17D5
