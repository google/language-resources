# Wikipedia Pronunciation Data Tables Extraction Project

This is not an official Google product.

## Background

The English Wikipedia has many pages on different languages containing
pronunciation information in table format, which we have automatically extracted
and formatted to make the data more amenable to automatic processing. The
information includes grapheme to phoneme mappings (g2p) in some cases, phoneme
inventories in some cases, or some combination of phonemes, phonetic features,
example words containing the phonemes, or other pronunciation details.

The data is extracted from the tables found in 816 Wikipedia language pages. The
pages targeted are the ones found in the Category pages for "Languages by
country" and "Language Phonologies"
(https://en.wikipedia.org/wiki/Category:Languages_by_country,
https://en.wikipedia.org/wiki/Category:Language_phonologies). Any other
languages could be added by appending their ISO-639 code to the query
"https://en.wikipedia.org/wiki/ISO_639:".

Note that this data set does not include IPA pronunciations which may be listed
in the text of a Wikipedia page, only data extracted from formatted tables.

For more details about how this data was extracted, see our paper [Mining
Large-Scale Low-Resource Pronunciation Data from
Wikipedia](https://research.google/pubs/pub50056/)

## Structure of this Dataset

The list of Wikipedia pages that were mined for this dataset are found in the
wikilinks.txt file.

Data for each language can be found in a tsv file named like ISO639Code.tsv,
i.e. "oca.tsv" for the Ocaina language. Each tab-separated row represents:

G | P | F | EX | IPA | BG

|: [tab]

G: grapheme

P: phoneme

F: a pronunciation feature of the phoneme

EX: an example word containing the grapheme

IPA: IPA transcription of the example word

BG: a "best guess" of what some found data is if it couldn't be classified

If a value is missing (because the Wikipedia table didn’t contain a relevant
value, for example) that column is substituted with a "(n/a)".

### Examples:

From aaz.tsv; we have grapheme to phoneme mappings (g2p), but no other data.

j | [d͡ʒ] | (n/a) | (n/a) | (n/a) | (n/a)

From yko.tsv; we don't have any g2p mapping for this language but at least a
list of phonemes found in the language with associated features.

(n/a) | dʒ | Post-alveolar(Palato- alveolar) | (n/a) | (n/a) | (n/a)

From sq.tsv; the table that we parsed had an unusual format, so we weren't able
to confidently extract the g2p automatically but for the phoneme /b/ the Best
Guess column shows that 'b' is mapped to Latin, so is likely the grapheme used
for the phoneme in the Latin script.

(n/a) | /b/ | (n/a) | (n/a) | (n/a) | b -> Latin

## Data Statistics

Most of the languages that have more useful information in this set are the
lower-resource languages, as the Wikipedia pages for higher-resource languages
do not have the simply formatted tables containing phoneme and/or grapheme data;
they tend to have more in-depth content instead.

Number of langauges represented: 816

Number of languages that include g2p mappings: 63 (mostly low-resource)

Number of languages with a Best Guess estimate made: 224

## License

This dataset is released under CC-BY-SA:
https://creativecommons.org/licenses/by-sa/3.0/legalcode
