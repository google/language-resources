# Language Resources and Tools

[![Build Status](https://travis-ci.org/google/language-resources.svg?branch=master)](https://travis-ci.org/google/language-resources)

Datasets and scripts for basic natural language and speech processing.

This is not an official Google product.


## Natural Languages

| Directory | Language Available |
|-----------|--------------------|
| af        | Afrikaans          |
| bn        | Bengali / Bangla   |
| hi_ur     | Hindi & Urdu       |
| is        | Icelandic          |
| jv        | Javanese           |
| km        | Khmer              |
| lo        | Lao                |
| my        | Burmese / Myanmar  |
| ne        | Nepali             |
| si        | Sinhala            |
| su        | Sundanese          |
| xh        | Xhosa              |
| zu        | Zulu               |


## Tools

We are including a few tools for working with the natural language
datasets. These tools are written in C++ and Python and are built with
[Bazel](http://bazel.io). To compile and use these tools,
[install a recent version of Bazel](http://bazel.io/docs/install.html)
(minimally Bazel release 0.4.5 is required).

## Opensourced Audio Data
| Resource | Link |
|-----------|--------------------|
| Sinhala TTS recordings (~3K) | [https://www.openslr.org/30/](https://www.openslr.org/30/)      |
| TTS recordings for four South African languages (af, st, tn, xh) | [https://www.openslr.org/32/](https://www.openslr.org/32/)  |
| Large Javanese ASR training data set (~185K)	| [https://www.openslr.org/35/](https://www.openslr.org/35/) |
| Large Sundanese ASR training data set	(~220K)| [https://www.openslr.org/36/](https://www.openslr.org/36/) |
| High quality TTS data for Bengali languages | [https://www.openslr.org/37/](https://www.openslr.org/37/) |
| High quality TTS data for Javanese | [https://www.openslr.org/41/](https://www.openslr.org/41/) |
| High quality TTS data for Khmer | [https://www.openslr.org/42/](https://www.openslr.org/42/) |
| High quality TTS data for Nepali | [https://www.openslr.org/43/](https://www.openslr.org/43/) |
| High quality TTS data for Sundanese | [https://www.openslr.org/44/](https://www.openslr.org/44/) |
| Large Sinhala ASR training data set | [https://www.openslr.org/52/](https://www.openslr.org/52/)|
| Large Bengali ASR training data set	|  [https://www.openslr.org/53/](http://www.openslr.org/53/)|
| Large Nepali ASR training data set	| [https://www.openslr.org/54/](https://www.openslr.org/54/) |
| Crowdsourced high-quality Argentinian Spanish speech data set | [https://www.openslr.org/61/](https://www.openslr.org/61/) |
| Crowdsourced high-quality Malayalam multi-speaker speech data set | [https://www.openslr.org/63/](https://www.openslr.org/63/) |
| Crowdsourced high-quality Marathi multi-speaker speech data set | [https://www.openslr.org/64/](https://www.openslr.org/64/) |
| Crowdsourced high-quality Tamil multi-speaker speech data set | [https://www.openslr.org/65/](https://www.openslr.org/65/) |
| Crowdsourced high-quality Telugu multi-speaker speech data set | [https://www.openslr.org/66/](https://www.openslr.org/66/) |


## Other reading resources

SLTU 2016 Tutorial - [https://sites.google.com/site/sltututorial/overview](https://sites.google.com/site/sltututorial/overview)

## Publications

- [Text Normalization for Bangla, Khmer, Nepali, Javanese, Sinhala, and Sundanese TTS Systems](https://ai.google/research/pubs/pub47344)

- [A Step-by-Step Process for Building TTS Voices Using Open Source Data and Framework for Bangla, Javanese, Khmer, Nepali, Sinhala, and Sundanese](https://ai.google/research/pubs/pub47347)

- [Building Open Javanese and Sundanese Corpora for Multilingual Text-to-Speech](https://ai.google/research/pubs/pub46929)

- [Building Statistical Parametric Multi-speaker Synthesis for Bangladeshi Bangla](https://ai.google/research/pubs/pub45301)

- [TTS for Low Resource Languages: A Bangla Synthesizer](https://ai.google/research/pubs/pub45300)

## License

Unless otherwise noted, all original files are licensed under an
[Apache License, Version 2.0](LICENSE).

Where specifically noted, some datasets are licensed under a
[Creative Commons Attribution 4.0 International License (CC BY 4.0)](http://creativecommons.org/licenses/by/4.0).

The directory [third_party/](third_party/) contains third-party works, which we
are including under the respective licenses of the upstream projects. See [third_party/README.md](third_party/README.md) for further details.
