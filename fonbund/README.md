# FonBund: A Library for Combining Cross-lingual Phonological Segment Data

FonBund is a library that provides a way of mapping sequences of arbitrary
phonetic segments in International Phonetic Association (IPA) alphabet into
multiple articulatory feature representations. The library interfaces
with several existing linguistic typology resources providing phonological
segment inventories and their corresponding articulatory feature systems.

## Rationale

The first goal is to facilitate the derivation of articulatory features
without giving a special preference to any particular phonological segment
inventory provided by freely available linguistic typology resources.
The second goal is to build a very light-weight library that can be easily
modified to support new phonological segment inventories. In order to support
IPA segments unsuppored by the freely available resources the library provides
a simple configuration language for performing segment rewrites and adding custom
segments with the corresponding feature structures.

## Supported cross-lingual phonological databases

Several phonological segment repositories are supported by FonBund. These
are located elsewhere and we rely on the remote dependency mechanism of [Bazel](https://bazel.build/)
to pull the database sources from the remote repositories.

### PHOIBLE

PHOIBLE [http://phoible.org/](http://phoible.org/) ([Moran & McCloy, 2014](https://www.linguistlist.org/LL/fyi/fyi-details.cfm?submissionid=35958257)) is a
vast database that contains (according to the 2014 edition) 2155 phonological
inventories for 1672 distinct languages (some languages have more than one
description available) covering 2160 segment types. The phoneme inventories
are accompanied by a feature system that describes the segments in terms
of 37 features that take "+", "-", "0" and "" (absent) values.

### PanPhon

PanPhon [https://github.com/dmort27/panphon](https://github.com/dmort27/panphon)
([Mortensen, Littell, Bharadwaj, Goyal, Dyer & Levin, 2016](https://aclanthology.info/papers/C16-1328/c16-1328))
is resource consisting of a database that relates over 5,000 IPA
segments (simple and complex) to their definitions in terms of about 23 articulatory
features and a Python package to manipulate the segments and their feature
representations. Unlike PHOIBLE, which documents
the actual snapshot of modern phonological knowledge of world's languages from
the standpoint of linguistic theory, PanPhon's mission is to develop a
methodologically solid resource to facilitate research in NLP.

### `phon-class-counts` (Fonetikode for PHOIBLE)

The `phon-class-counts` [https://github.com/ddediu/phon-class-counts](https://github.com/ddediu/phon-class-counts)
database ([Dediu & Moisik, 2015](http://pubman.mpdl.mpg.de/pubman/item/escidoc:2327732/component/escidoc:2327739/Dediu_Moisik_LREC_2016_151_Paper.pdf)) provides a multi-valued feature encoding, called *Fonetikode*, for several
phonlogical segment inventories. At present we support Fonetikode feature
system representation covering the PHOIBLE database.

## License

The code is distributed under [Apache License Version 2.0](https://github.com/googlei18n/language-resources/blob/master/LICENSE).
