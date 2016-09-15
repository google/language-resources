# Myanmar Unicode Keyboard

Experimental code sample of a Myanmar composing keyboard with conversion from Zawgyi font encoding to Unicode.

This is not an official Google product.

## Features

* On-screen ("soft") keyboard for Burmese following Myanmar3 layout.
* [Key character map](res/raw/keyboard_layout_burmese.kcm) for physical keyboard, overlaying Myanmar3 layout onto a QWERTY keyboard.
* Incremental composition of grapheme clusters.
* Automatic correction of non-standard input. One example of many: typing သ (101E) followed by ြ (103C) gets corrected to ဩ (1029).
* Stand-alone library for composing grapheme clusters and serializing them in canonical storage order.
* The [grapheme composer library](../my/java/com/google/languageresources/my/GraphemeComposer.java) is configurable: decide at construction time whether ေ (1031) must be typed before or after a letter (in the sample keyboard, it must be typed before a letter), whether ြ (103C) must be typed before or after a letter, etc.
* Soft key for conversion from Zawgyi. Converts Android clipboard contents from Zawgyi to Unicode and inserts the resulting Unicode text.
* Physical keyboard shortcut (Ctrl-Shift-V) for conversion from Zawgyi.
* Stand-alone [Zawgyi-to-Unicode conversion](../my/java/com/google/languageresources/my/ZawgyiToUnicodeConverter.java) Java library and command-line utility.

## Build instructions

Building the code sample currently requires [Bazel](https://www.bazel.io/). Support for Gradle may be added later. The [Android SDK](https://developer.android.com/studio/index.html) command line tools need to be installed under `/usr/local/android-sdk` (symlink is fine), together with build-tools and android-24 platform. Details can be found in [.travis.yml](../.travis.yml).

The build has been tested on Darwin and GNU/Linux (including with Travis CI).

## License

All files are licensed under an [Apache License, Version 2.0](../LICENSE).
