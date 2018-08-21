# Language Resources and Tools

[![Build Status](https://travis-ci.org/googlei18n/language-resources.svg?branch=master)](https://travis-ci.org/googlei18n/language-resources)

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
| lo        | Lao                |
| my        | Burmese / Myanmar  |
| si        | Sinhala            |
| xh        | Xhosa              |
| zu        | Zulu               |


## Tools

We are including a few tools for working with the natural language
datasets. These tools are written in C++ and Python and are built with
[Bazel](http://bazel.io). To compile and use these tools,
[install a recent version of Bazel](http://bazel.io/docs/install.html)
(minimally Bazel release 0.2.0 is required).


## License

Unless otherwise noted, all original files are licensed under an
[Apache License, Version 2.0](LICENSE).

Where specifically noted, some datasets are licensed under a
[Creative Commons Attribution 4.0 International License (CC BY 4.0)](http://creativecommons.org/licenses/by/4.0).

The directory [third_party/](third_party/) contains third-party works, which we
are including under the respective licenses of the upstream projects. See [third_party/README.md](third_party/README.md) for further details.
