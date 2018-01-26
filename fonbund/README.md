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

## Example use

### Inspecting segment repositories

The [show_segments.py](https://github.com/googlei18n/language-resources/blob/fonbund/fonbund/show_segments.py)
utility can be used to display the any of the currently supported segment repositories. For example, to
display the current contents of the PHOIBLE Fonetikode repository, run

```shell
bazel build fonbund:show_segments

bazel-bin/fonbund/show_segments -db phoible_fonetikode
```

The above command will dump the segments and their corresponding features in
PHOIBLE Fonetikode to standard output:

```shell
ID	Name General Class V-V. V-H. V-C. C-P. C-M. C-S. Phon. Init. Pri. 2nd Pros. Tone. Notes
276	d̤ c    a s s b p none none none none
2520	d̤ɮ̤ c    a af c b p none l none none
1671	dz̤ c    a af c b p none none none none
...
```

### Retrieving articulatory features

Utility [features_for_segments.py](https://github.com/googlei18n/language-resources/blob/fonbund/fonbund/features_for_segments.py)
can be used for rewriting arbitrary strings of phonetic segments to their corresponding
articulatory feature representations in textual [protocol buffer](https://developers.google.com/protocol-buffers)
format. For example, to retrieve the features for one possible transcription of Danish word *stærkeste*
(`/sdæʌ̯ɡəsdə/`) in one unified representation using PanPhon, PHOIBLE and PHOIBLE Fonetikode systems, run

```shell
bazel build fonbund:features_for_segments

bazel-bin/fonbund/features_for_segments \
  --databases panphon,phoible,phoible_fonetikode \
  --segments s,d,æ,ʌ̯,ɡ,ə,s,d,ə \
  --output_features_file features.textproto \
  --logtostderr
```

The resulting file `features.textproto` will look as follows:

```shell
features {
  segment_name: "s"
  representation {
    source_name: "PanPhon"
    feature_names: "syl"
    feature_names: "son"
    ...
    feature_list {
      feature {
        binary_value: NEGATIVE
      }
      feature {
        binary_value: NEGATIVE
      }
      ...
    }
  }
  representation {
    source_name: "PHOIBLE"
    feature_names: "tone"
    feature_names: "stress"
    ...
    feature_list {
      feature {
        binary_value: NOT_APPLICABLE
      }
      feature {
        binary_value: NEGATIVE
      }
      ...
    }
  }
}
features {
  segment_name: "d"
  ...
  }
  ...
}
...
```

For each phonetic segment in the transcription, there is a protocol buffer
message containing unified articulatory feature description of this segment
in all the configured feature systems.

### Using the API

A reasonably high-level API is provided by the `segment_to_features_converter`
library. Following snippet demonstrates how to initialize the feature extractor
from a custom phonological segment database and corresponding configuration
file:

```python
import os

from fonbund import distinctive_features_pb2 as df
from fonbund import segment_to_features_converter as converter_lib

def convert_segments(segments):
  custom_segments_db_file_path = "/some/path/file.tsv"
  custom_config_file_path = "/some/path/config.textproto"
  converter = converter_lib.SegmentToFeaturesConverter()
  assert converter.OpenPaths(custom_config_file_path, [custom_segments_db_file_path]))
  status, features = converter.ToFeatures(segments)
  assert status
  # The "features" is a list of "df.DistinctiveFeatures" protocol buffer
  # messages each corresponding to a segment.
  assert len(features) == len(segments)
```

## License

The code is distributed under [Apache License Version 2.0](https://github.com/googlei18n/language-resources/blob/master/LICENSE).
