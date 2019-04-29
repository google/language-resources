# Argentinian Spanish CTC phoneme lattices

This directory contains CTC posterior labeling distributions for a Spanish
phoneme recognition task. These were generated using the
[Persephone](https://github.com/persephone-tools/persephone) toolkit trained on
a subset of a [crowdsourced high-quality Argentinian Spanish speech data
set](https://www.openslr.org/61/) and evaluated on a small held-out subset.

The labeling distributions are presented in two equivalent formats:

* In the directory `logits` as 2-dimensional NumPy arrays of shape (F, L) where
  F is the number of frames and L is the size of the labeling alphabet. Each row
  holds the per-frame logits of the L-dimensional categorical distribution over
  labels.
* In the directory `fsts` as serialized finite automata in
  [OpenFst](http://openfst.org/) format.
