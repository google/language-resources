# Bangla phonemic transcription guide

This document sets out guidelines for the phonemic transcription of colloquial Bangla, in particular the standard language used in Bangladesh. The goal of phonemic transcription is to provide an accurate representation of the contrasting segments (phonemes) of the language. This will allow a computer (or an absolute beginner learning Bangla as a second language) to pronounce words or sentences correctly.


## Transcription conventions

The conventions proposed here aim to establish transcriptions that are concise, are able to represent the idiosyncratic details of pronunciation that cannot be gleaned from the orthography of words, and are both human- and machine-readable.

Bangla spelling is largely phonemic. However, not all details of the pronunciation of a word are fully indicated by the writing system. For example, the letter এ can have two different pronunciations, depending on context: in the word এক, the letter এ is more open (similar to the beginning of the English word "act") than in the word একে. The goal of transcription is to resolve the important distinctions in pronunciations that are not fully transparent from the spelling of a word.

At the same time, our goal is broad phonemic transcription, representing only
the contrastive sounds (phonemes) of Bangla. It is not our goal to define a
narrow phonetic transcription, which may represent invidual or regional
variations that are not distinctive.

As a general principle, we put spaces between phonemes and mark syllable
boundaries by a dot (period, full stop). For example সিলেবল is transcribed /s i
. l e . b o l/ to indicate that it consists of 3 syllables.


## Phonemes

We describe the phonemes one by one, using examples taken from Khan 2010, Radice 2010, and other sources listed under References below.

### Oral vowels

Bangla has a system of 7 simple vocalic contrasts:

| Phoneme | Bangla letter | Example | Transcription | Gloss |
|---------|---------------|---------|---------------|-------|
| i | ই | দিস | d i sh | you give |
| u | উ | দু'শ | d u . sh o | two hundred |
| e | এ | দেশ | d e sh | country |
| o | ও | দোষ | d o sh | blame |
| E | অ্যা | দেখ্ | d E kh | look! |
| O | অ | দশ | d O sh | ten |
| a | আ | দাস | d a sh | slave |

It is important to pay special attention to the distinction between the mid vowels /o/ vs. /O/; and /e/ vs. /E/. This is especially true when the /o/ or /O/ corresponds to the intrinsic vowel of a consonant letter (when there is no explicit vowel sign). There is always a question whether an implicit vowel should be transcribed as /o/, as /O/, or is completely absent. Consider the vowel (if any) after ক in these examples:

| Example | Transcription | Gloss |
|---------|---------------|-------|
| টক | T O k | sour |
| করুণ | k o . r u n | sad |
| করা | k O . r a | to do |
| সে করে | s e . k O . r e | he/she does |
| ভালো করে | bh a . l o . k o . r e | well |

The letter এ can be read either /e/ or /E/, depending on the word or context:

| Example | Transcription | Gloss |
|---------|---------------|-------|
| এক | E k | one |
| একে | e . k e | him/her |
| সে দেখে | s e . d E . kh e | he/she sees |
| আমি দেখে এসেছি | a . m i . d e . kh e . e . s e . ch i | |


### Semi-vowels as off-glides (falling diphthongs)

Diphthongs (e.g. arising in the inflection paradigm of vowel-stem verbs) are
transcribed as indicated below, largely following the orthography. (It is
unclear at this point which contrasts are actually phonemic.)

| Phoneme | Example | Transcription | Gloss |
|---------|---------|---------------|-------|
| i^ | সুই | sh u i^ | needle |
|    | নেই | n e i^ | I take |
|    | সই | sh o i^ | signature |
|    | পাই | p a i^ | I find |
| u^ | পিউ | p i u^ | Piu (name) |
|    | ঢেউ | Dh e u^ | wave |
|    | বউ | b o u^ | bride |
|    | পাউ | p a u^ | sliced bread |
| e^ | শোয় | sh o e^ | lies down |
|    | ন্যায় | n E e^ | justice |
|    | সয় | sh O e^ | tolerates |
|    | পায় | p a e^ | finds |
| o^ | শোও | sh o o^ | you lie down |
|    | নেও | n E o^ | you take |
|    | সও | sh O o^ | you tolerate |
|    | পাও | p a o^ | you find |

Note that we distinguish a monosyllabic word (involving a diphthong) like শোয় /sh o e^/ from a bisyllabic word (involving two adjacent vowels) like সয়ে /sh o . e/.


### Semi-vowels as on-glides (rising diphthongs)

This table is incomplete.

| Phoneme | Example | Transcription | Gloss |
|---------|---------|---------------|-------|
| i^ | ইউরোপীয় | i^ u . r o . p i . o | European |
|    | ইয়াহু | i^ a . h u | Yahoo! |
|    | ইয়েমেন | i^ e . m e n | Yemen |
|    | রয়েছে | r o . i^ e . ch e | |
| o^ | সফটওয়্যার | s O f T . o^ E r | software |
|    | ওয়াশিংটন | o^ a . sh i N . T O n | Washington |

Note that the vocalic sequence in পিউ /p i u^/ contrasts with the vocalic
sequence at the start of ইউরোপীয় /i^ u . r o . p i . o/.  In পিউ /p i u^/ the main vowel of the syllable is /i/ followed by an off-glide, whereas in ইউ /i^ u/ at the start of ইউরোপীয় the main vowel of the syllable is /u/ preceded by an on-glide.


### Nasal vowels

Bangladeshi Standard Bengali does not appear to have nasal vowels. The written
contrast between তারা ("they", familiar) and তাঁরা ("they", polite) is absent
from colloquial speech: when nasalization is indicated in the orthography, this
is effectively ignored and the plain oral vowel is used instead. We do not
transcribe nasalization of vowels. (It is possible to recover nasal vowels, if
desired, from a combination of the phonemic transcription plus orthography.)


### Stops

| Phoneme | Example | Transcription | Gloss |
|---------|---------|---------------|-------|
| k  | কাশ | k a sh  | wild sugarcane |
| kh | খাস | kh a sh | you eat |
| g  | গাস | g a sh  | you sing |
| gh | ঘাস | gh a sh | grass |
| c  | চাল | c a l | rice |
| ch | ছাল | ch a l | peel |
| j  | জাল | j a l | net |
| jh | ঝাল | jh a l | spicy |
| T  | টাক | T a k | bald spot |
| Th | ঠাকুর | Th a . k u r | lord |
| D  | ডাক | D a k | call! |
| Dh | ঢাক | Dh a k | cover! |
| t  | তাক | t a k | shelf |
| th | থাক | th a k | let it be |
| d  | দাগ | d a g | stain |
| dh | ধাক্কা | dh a k . k a | shove |
| p  | পাটি | p a . T i | grass mat |
| b  | বাটি | b a . T i | bowl |
| bh | ভাটি | bh a . T i | kiln |


### Nasals

| Phoneme | Example | Transcription | Gloss |
|---------|---------|---------------|-------|
| N | ডাঙার | D a N . a r | of dry land |
| n | আনার | a . n a r | for bringing |
|   | নাশ | n a sh | destruction |
| m | আমার | a . m a r | my |
|   | মাস | m a sh | month |


### Fricatives

| Phoneme | Example | Transcription | Gloss |
|---------|---------|---------------|-------|
| f  | ফাঁস | f a sh | noose |
| r  | হ্রাস | r a sh | reduction |
| l  | লাশ | l a sh | corpse |
| h  | হাঁস | h a sh | duck |
| s  | সাফ | s a f | clean |
|    | সির্কা | s i r . k a | vinegar |
|    | আস্তে | a . s t e | softly |
|    | ব্যস | b a s | enough |
| sh | শ্বাস | sh a sh | breath |
|    | সিরা | sh i . r a | syrup |
|    | আসতে | a sh . t e | to come |
|    | বাঁশ | b a sh | bamboo |

The contrast between /s/ and /sh/ appears to be phonemic in Bangladeshi Bengali, by and large. However, it remains to be seen how stable it is between and within individual speakers.


### Geminates

Geminates occur ambisyllabically. We represent them as a sequence of two
segments, with an intervening syllable boundary. For unaspirated stops, nasals,
and fricatives, a phoneme is repeated identically to indicate a geminate:

| Phoneme | Example | Transcription | Gloss |
|---------|---------|---------------|-------|
| k   | টিকা | T i . k a | vaccine |
| k.k | টিক্কা | T i k . k a | kabob |
| g   | ভাগো | bh a . g o | you flee |
| g.g | ভাগ্য | bh a g . g o | luck |
| c   | বাঁচা | b a . c a | surviving |
| c.c | বাচ্চা | b a c . c a | child |
| j   | সোজা | sh o . j a | straight |
| j.j | শয্যা | sh o j . j a | bed |
| T   | খাটা | kh a . T a | toiling |
| T.T | খাট্টা | kh a T . T a | sour |
| D   | সোডা | s o . D a | soda |
| D.D | আড্ডা | a D . D a | meeting place |
| t   | আতা | a . t a | custard apple |
| t.t | আত্মা | a t . t a | soul |
| d   | বিদায় | b i . d a e^ | farewell |
| d.d | বিদ্যায় | b i d . d a e^ | in wisdom |
| n   | কানা | k a . n a | half-blind |
| n.n | কান্না | k a n . n a | weeping |
| p   | খাপা | kh a . p a | shrinking |
| p.p | খাপ্পা | kh a p . p a | enraged |
| b   | আবার | a . b a r | again |
| b.b | আব্বার | a b . b a r | dad's |
| m   | সমান | sh O . m a n | equal |
| m.m | সম্মান | sh O m . m a n | respect |
| l   | তুলো | t u . l o | you pick up |
| l.l | তুল্য | t u l . l o | comparable |
| sh  | বিশে | b i . sh e | twentieth day |
|     | দোষী | d o . sh i | guilty |
| sh.sh | বিশ্বে | b i sh . sh e | in the world |
|       | দস্যি | d o sh . sh i | rogue |

To transcribe geminated aspirated stops, the corresponding unaspirated stop is
placed before the aspirated stop:

| Phoneme | Example | Transcription | Gloss |
|---------|---------|---------------|-------|
| kh   | শাখা | sh a . kh a | branch |
| k.kh | সাক্ষাৎ | sh a k . kh a t | meeting |
| ch   | বিছা | b i . ch a | spread! |
| c.ch | ইচ্ছা | i c . ch a | desire |
| jh   | বুঝো | b u . jh o | you understand |
| j.jh | সহ্য | sh o j . jh o | tolerance |
| Th   | পাঠা | p a . Th a | send! |
| T.Th | পাঠ্য | p a T . Th o | text |
| th   | মাথা | m a . th a | head |
| t.th | মিথ্যা | m i t . th a | falsehood |
| dh   | বাধো | b a . dh o | you resist |
| d.dh | বাধ্য | b a d . dh o | obliged |
| bh   | শুভ | sh u . bh o | auspicious |
| b.bh | সভ্য | sh o b . bh o | civilized |


## Notes on spelling-sound correspondence

The same sound can be represented differently in the orthography. The
distinction in the writing system need not and **should not be preserved** in
phonemic transcription, which should solely represent the idealized
pronunciation of words and phrases.

An incomplete list of examples of phonemes with multiple spelling
representations is shown below. The important thing to note is that the sounds
are transcribed the same, regardless of the spelling of the words.

| Phonemes | Example | Transcription | Gloss |
|----------|---------|---------------|-------|
| N    | ডাঙার | D a N . a r | of dry land |
|      | বাংলা | b a N . l a | Bangla |
| n    | গঞ্জ | g O n j | marketplace |
|      | ঘণ্টা | gh O n . T a | hour |
|      | গন্ধ | g O n . dh o | scent, smell |
| j    | জখম | j O . kh o m | wound |
|      | যখন | j O . kh o n | when |
|      | জ্বলা | j O . l a | to burn, blaze |
| o u^ | বউ | b o u^ | bride |
|      | পৌঁছাবে | p o u^ . ch a . b e | |


## References

* Firoj Alam, S. M. Murtoza Habib, Mumit Khan. 2008. [Acoustic analysis of Bangla vowel inventory](http://dspace.bracu.ac.bd/bitstream/handle/10361/643/Acoustic%20Analysis%20of%20Bangla%20Vowel%20inventory%20.pdf). Technical report. Center for Research on Bangla Language Processing, BRAC University.
* Firoj Alam, S. M. Murtoza Habib, Mumit Khan. 2008. [Acoustic analysis of Bangla consonants](http://www.mica.edu.vn/sltu2008/article/Article%2020.pdf). In Proceedings of the first International Workshop on Spoken Languages Technologies for Under-resourced languages (SLTU), Hanoi, Vietnam. pp. 108-113.
* Firoj Alam, S. M. Murtoza Habib, Mumit Khan. 2011. [Bangla text to speech using Festival](http://www.cle.org.pk/hltd/pdf/HLTD201125.pdf). In Proceedings
of the Conference on Human Language Technology for Development (HLTD), Alexandria, Egypt, pp. 154-161.
* Sameer ud Dowla Khan. 2010. [Bengali (Bangladeshi Standard)](http://www.reed.edu/linguistics/khan/assets/Khan%202010%20Bengali%20Bangladeshi%20standard.pdf). _Journal of the International Phonetic Association_ 40(2), 221-225.
* Ayesha Binte Mosaddeque, Naushad UzZaman, Mumit Khan. 2006. [Rule based automated pronunciation generator](https://sites.google.com/a/naushadzaman.com/www/RAPG_ICCIT06.pdf). In Proceddings of 9th International Conference on Computer and Information Technology (ICCIT), Dhaka, Bangladesh.
* William Radice. 2010. _[Complete Bengali](https://en.wikipedia.org/w/index.php?title=Special%3ABookSources&isbn=9781444106060)_ (4th edition, 2010). Hodder Education, London.
