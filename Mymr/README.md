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
