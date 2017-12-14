#! /usr/bin/env python

"""Checks that all words in a prompt file occur in the corresponding lexicon.
"""

import codecs
import io
import sys


STDOUT = io.open(1, 'wb')
STDERR = io.open(2, 'wb')


def Print(*objects, **kwargs):
  sep = kwargs.get('sep', u' ')
  end = kwargs.get('end', u'\n')
  buf = kwargs.get('file', STDOUT)
  s = sep.join(u'%s' % o for o in objects)
  s += end
  buf.write(s.encode('utf-8'))
  if kwargs.get('flush', False):
    buf.flush()
  return


def main(argv):
  if len(argv) <= 2:
    Print('Usage: %s prompts.tsv lexicon.tsv' % argv[0], file=STDERR)
    sys.exit(2)

  prompts_path = argv[1]
  lexicon_path = argv[2]

  vocabulary = set()
  with codecs.open(lexicon_path, 'r', 'utf-8') as reader:
    for line in reader:
      line = line.rstrip('\r\n')
      if not line or line.startswith('#'):
        continue
      fields = line.split('\t')
      vocabulary.add(fields[0])
  Print('Found %d unique words in lexicon.' % len(vocabulary), file=STDERR)

  success = True
  n = 0
  with codecs.open(prompts_path, 'r', 'utf-8') as reader:
    for line in reader:
      line = line.rstrip('\r\n')
      n += 1
      fields = line.split('\t')
      assert len(fields) >= 2
      for word in fields[1].split():
        if word not in vocabulary:
          success = False
          Print('\t'.join(['OOV', word] + fields))
  Print('Processed %d prompts.' % n)

  if success:
    Print('PASS')
    sys.exit(0)
  else:
    Print('FAIL')
    sys.exit(1)


if __name__ == '__main__':
  main(sys.argv)
