# Myanmar Unicode Keyboard

Experimental code sample of a Myanmar composing keyboard with conversion from Zawgyi font encoding to Unicode.

This is not an official Google product.

## Features

* On-screen ("soft") keyboard for Burmese following Myanmar3 layout.
* Key character map for physical keyboard, overlaying Myanmar3 layout onto a QWERTY keyboard.
* Incremental composition of grapheme clusters.
* Stand-alone library for composing grapheme clusters and serializing them in canonical storage order.
* Soft key for conversion from Zawgyi. Converts clipboard contents from Zawgyi to Unicode and inserts the resulting Unicode text.
* Physical keyboard shortcut (Ctrl-Shift-V) for conversion from Zawgyi.
* Stand-alone library for Zawgyi-to-Unicode conversion.

## Build instructions

Building the code sample currently requires [Bazel](https://www.bazel.io/). Support for Gradle may be added later. The [Android SDK](https://developer.android.com/studio/index.html) command line tools need to be installed under `/usr/local/android-sdk` (symlink is fine), together with build-tools and android-24 platform. Details can be found in [.travis.yml](../.travis.yml).

The build has been tested on Darwin and GNU/Linux (including with Travis CI).

## License

All files are licensed under an [Apache License, Version 2.0](../LICENSE).
