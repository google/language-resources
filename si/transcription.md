# Sinhala phonemic transcription guide

This document sets out guidelines for the phonemic transcription of colloquial
Sinhala. The goal of phonemic transcription is to provide an accurate
representation of the contrasting segments (phonemes) of the language. This
will allow a computer (or an absolute beginner learning Sinhala as a second
language) to pronounce words or sentences correctly.

We focus on several areas in which the pronunciation of Sinhala words is
either not fully predictable from their orthography, or in which the
pronunciation differs from what would be expected from the orthography. It is
these areas of ambiguity or uncertainty that require intervention by human
transcribers.

These guidelines do not cover the full details of Sinhala phonology and the
grapheme-phoneme correspondence. We assume that a rough transcription can be
obtained by [deterministic pronunciation rules](si-si_FONIPA.txt), so that
human transcribers can focus on correcting the automatic transcriptions,
especially with respect to the areas of ambiguity explained below.


## Sinhala phonemes

We use an inventory of 40 phonemes together with "." (full stop, dot), which
indicates syllable boundaries. The letters in the following table are only
meant for illustration: sometimes the same letter can be read in different
ways; and many phonemes can be indicated by more than one Sinhala letter.

All phonemes are represented using the International Phonetic Alphabet
(IPA). In addition, the phoneme /j/ can optionally be written as /y/, which is
closer to how Sinhala would conventionally be Romanized and which is less
likely to be confused with the phoneme /ɟ/.

Phoneme | Typical Sinhala letter
--------|-----------------------
ə  | see ["The inherent vowel"](#the-inherent-vowel)
əː | see ["The inherent vowel"](#the-inherent-vowel)
a  | see ["The inherent vowel"](#the-inherent-vowel)
aː | ආ
æ  | ඇ
æː | ඈ
i  | ඉ
iː | ඊ
u  | උ
uː | ඌ
e  | එ
eː | ඒ
o  | ඔ
oː | ඕ
k  | ක්
g  | ග්
ŋ  | ඞ්
ᵑg | ඟ්
c  | ච්
ɟ  | ජ්
ɲ  | ඤ්
ʈ  | ට්
ɖ  | ඩ්
ⁿɖ | ඬ්
t  | ත්
d  | ද්
n  | න්
ⁿd | ඳ්
p  | ප්
b  | බ්
m  | ම්
ᵐb | ඹ්
j/y | ය්
r  | ය්
l  | ල්
w  | ව්
ʃ  | ෂ්
s  | ස්
h  | හ්
f  | ෆ්

For example, the word "සංස්කෘතිය" would be transcribed phonemically as
/saŋs.kru.ti.jə/. The dots indicate that the word consists of four
syllables. (We sometimes optionally use spaces to separate phonemes. This
results in the same pronunciation, i.e. spaces are not significant.)


## Historic syllabic consonants

The modern Sinhala pronunciation of the historic syllabic consonants ඍ  ෘ
(gæṭa yanna/pilla) and ඎ  ෲ (diga gæṭa yanna/pilla) is not fully predictable
(Wasala et al. 2006, p. 895). At the beginning of a word ඍ is often read
/ri/. In non-initial position,  ෘ can be read /ru/ or /ur/.

The following examples are taken from Wasala et al. 2006:

Orthography | Transcription | Gloss
------------|---------------|------
ඍණ | ri.nə | minus
කෘතිය | kru.ti.jə | book
විවෘත | wi.wur.tə | opened

When  ෘ is read /ur/, this may trigger further cluster simplification (to avoid
coda clusters in native words), which needs to be transcribed
approriately. Examples include:

Orthography | Expected but incorrect transcription | Correct transcription
------------|--------------------------------------|----------------------
සමෘද්ධි | ~~sa.murd.di~~ | sa.mur.di
ප්‍රවෘත්ති | ~~pra.wurt.ti~~ | pra.wur.ti


## æda pilla

The pronunciation of  ැ (æda pilla) and  ෑ (diga æda pilla) is not fully
predictable. The vowel stroke  ැ (æda pilla) can be read either /æ/ (common)
or /u/ (less common). Analogously,  ෑ can be read /æː/ or /uː/.

The following examples are based on Wasala et al. 2006:

Orthography | Transcription | Gloss
------------|---------------|------
කැවුම | kæw.mə | traditional sweet
ජනශ්‍රැති | ɟa.nə.ʃru.ti | legend, folklore
රෑයියා | ræːj.jaː | insect
ක්‍රෑර | kruː.rə | cruel


## Diphthongs

We transcribe diphthongs as a combination of a main vowel followed by a glide
within the same syllable.

Views on the nature of diphthongs in Sinhala differ widely. Masica (p. 116)
states unequivocally that "Sinhalese [...] has no diphthongs". This contrasts
with accounts that posit up to 20 diphthongs as separate phonemes. This much
is clear: any of the 12 main vowels (excluding the schwas, /ə/ and /əː/) can
be followed by an off-glide (either /j/ or /w/) in syllable coda
position. Since virtually all combinations of vowels and glides can occur in
principle, it is simplest to allow them to combine freely in
transcription. Examples of vowel+glide sequences include:

Diphthong | Example | Transcription | Gloss | Notes
----------|---------|---------------|-------|------
uj  | රුයිත | ruj.tə | diamond | යි
ej  | මැරෙයි | mæ.rej | get killed | යි
oj  | කොයි | koj | where | යි
æj  | ඇයි | æj | why | යි
aj  | කෛරාටික | kaj.raː.ʈi.kə | cunning | ඓ
uːj | ඌයි | uːj | hurt | යි
eːj | ඒයි | eːj | to call someone | යි
oːj | හෝයිය | hoːj.ja | take an effort | යි
æːj | රෑයියා | ræːj.jaː | insect | යි
aːj | ආයි | aːj | again | යි
iw  | ලිව්වා | liw.waː | write | ව්
ew  | කෙරෙව්වා | ke.rew.waː | had done | ව්
ow  | සොරොව්ව | so.row.wə | sluice gate | ව්
æw  | කැවුම | kæw.mə | traditional sweet | වු
aw  | කෞතුක | kaw.tu.kə | relic | ඖ
iːw | මීව්වා | miːw.waː | buffalo calf | ව්
eːw | දේව්දාස් | deːw.daːs | | ව්
oːw | හෝව් | hoːw | stop | ව්
æːw | හෑව්වා | hæːw.waː | plough | ව්
aːw | ලාවුලු | laːw.lu | fruit | වු

Note that the same diphthong can be written in different ways in Sinhala
orthography. For example, the diphthong /aj/ can be written as follows in
different words:

Example | Transcription | Notes
--------|---------------|------
කෛරාටික | kaj.raː.ʈi.kə | ඓ
නම්‍යයි | nam.jaj | යි
අය්ය | aj.jə | ය්


## The inherent vowel

The main challenge for Sinhala phonemic transcription is the treatment of the
unwritten inherent vowel. This vowel occurs when a consonant letter is not
followed by a dependent vowel or an explicit virama (hal kirima). Wasala et
al. 2006 view this situation as one of vowel epenthesis, but this is slightly
misleading: the inherent vowel is virtually always realized when indicated in
the orthography, and Wasala et al. do not report other cases of epenthesis
(e.g. for cluster simplification). In fact the main question concerns the
quality of the inherent vowel: Is the inherent vowel realized as /ə/ or /a/
(both common) or as /əː/ (rare)?

Examples of the possible realizations of the independent vowel include:

Realization | Example | Transcription | Gloss
------------|---------|---------------|------
ə  | කර | kə.rə | to do
a  | නම්‍යයි | nam.jaj | flexible
əː | පරණ | pa.rəː.nə | old

A similar issue arises for the reading of the independent vowel sign අ, which
can be realized as either /a/ (typically in native words) or /ə/ (typically in
certain foreign words). Examples of this include:

Realization | Example | Transcription | Gloss
------------|---------|---------------|------
ə | අර්න්ස්ට් | ərnsʈ | Ernst (a name)
a | අර්ජුන | ar.ɟu.nə | Arjuna (a name)


## References

* Dileep Chandralal. 2010. [Sinhala](http://dx.doi.org/10.1075/loall.15).
(London Oriental and African Language Library, 15.) John Benjamins.
* Colin P. Masica. 1991. [The Indo-Aryan
languages](https://books.google.com/books?id=Itp2twGR6tsC). (Cambridge language
surveys.) Cambridge University Press.
* H. S. Perera, Daniel Jones. 1919. [A colloquial Sinhalese reader in phonetic
transcription](https://archive.org/details/colloquialsinhal00pererich).
Manchester University Press; Longmans, Green & Co.
* Asanka Wasala, Ruvan Weerasinghe, Kumudu Gamage. 2006. [Sinhala
grapheme-to-phoneme conversion and rules for schwa
epenthesis](http://www.aclweb.org/anthology/P06-2114). COLING/ACL 2006,
pp. 890-897.
