#! /usr/bin/env python
#
# Copyright 2008 Google Inc. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Compare predicted pronunciations against a golden pronunciation
lexicon and report several evaluation metrics.
"""

__author__ = 'mjansche@google.com (Martin Jansche)'

import codecs
import math
import sys

import edist


_stdin  = codecs.lookup('utf_8')[2](sys.stdin)
_stdout = codecs.lookup('utf_8')[3](sys.stdout)
_stderr = codecs.lookup('utf_8')[3](sys.stderr)


_INF = 1e300 * 1e300
_NAN = _INF - _INF


class FLAGS:
  output_edit_stats = False
  output_word_stats = False
  output_summary_stats = True
  predicted_oracle_type = "best"


def fdiv(x, y):
  """By default, Python turns SIGFPE into an exception on floating
  point division-by-zero.  Work around it.
  """
  if x != x or y != y:
    return _NAN
  if y == 0:
    if x == 0:
      return _NAN
    elif x > 0:  # This is wrong when y == -0.0.
      return _INF
    else:
      return -_INF
  else:
    return x * 1.0 / y


def ReadLexicon(reader):
  """Reads a pronunciation lexicon from 'reader'.  The lexicon file
  format is line-oriented, where each line is of the form

    word TAB pronunciation [TAB cost [TAB field]...]

  and pronunciations are space-separated.  The third column gives an
  optional rank or cost (lower is better) that is used when multiple
  pronunciations per word are provided.
  """
  d = {}
  for line in reader:
    line = line.rstrip('\r\n')
    fields = line.split('\t')
    if len(fields) < 2:
      _stderr.write("Warning: Ignoring line with less than two "
                    + "tab-separated fields: %s\n" % line)
      continue
    word = fields[0]
    pron = fields[1].strip()
    if pron != fields[1]:
      _stderr.write("Info: Removed outer whitespace around pronunciation "
                    + "'%s' on line: %s\n" % (pron, line))
    if "  " in pron:
      _stderr.write("Warning: Two adjacent spaces found in pronunciation "
                    + "'%s' on line: %s\n" % (pron, line))
    pron = tuple(pron.split(' '))
    if len(fields) >= 3:
      try:
        cost = float(fields[2])
        other = tuple(fields[3:])
      except:
        _stderr.write("Warning: Ignoring line with ill-formed cost '%s': %s\n"
                      % (fields[2], line))
        continue
    else:
      assert len(fields) == 2
      other = ()
      # Special hack to assign increasing cost to multiple pronunciations
      # for a given word when no explicit cost is given.
      if word in d:
        max_cost = -_INF
        for _, cost, _ in d[word]:
          if cost > max_cost:
            max_cost = cost
        cost = max_cost + 1
        _stderr.write(("Info: Multiple pronunciations for word '%s' found,"
                       + " but cost was not specified for '%s'; using %g\n")
                      % (word, fields[1], cost))
      else:
        _stderr.write("Info: No cost specified, using 0: %s\n" % line)
        cost = 0
    if word in d:
      d[word].append((pron, cost, other))
    else:
      d[word] = [(pron, cost, other)]
  # Sort prons by increasing cost.
  for prons in d.itervalues():
    prons.sort(lambda x, y: cmp(x[1], y[1]))
  return d


class Stats:
  def __init__(self):
    self.not_found = 0
    self.words_total = 0
    self.prons = 0
    self.word_error = 0
    self.phone_edits = 0
    self.reference_length = 0
    self.phone_error = 0
    self.squared_phone_error = 0
    self.max_phone_error = -_INF
    self.reciprocal_rank = 0
    self.edit_stats = {}
    return


def CompareLexica(golden_lex, predicted_lex, writer,
                  golden_cutoff=None, predicted_cutoff=None):
  """Compares the predicted pronunciations against the golden
  reference dictionary.  Outputs statistics to 'writer' in
  tab-separated value format with headers, to facility further
  analysis with R.
  """
  stats = Stats()
  for word, prons in predicted_lex.iteritems():
    if word not in golden_lex:
      _stderr.write("Warning: Word '%s' not found in golden lexicon\n" % word)
      stats.not_found += 1
      continue
    else:
      stats.words_total += 1

    assert len(prons) >=1
    stats.prons += len(prons)

    assert len(golden_lex[word]) >= 1

    # Compute stats for phone error rate and word error rate:
    argvals = []
    for predicted in prons[:predicted_cutoff]:
      predicted_pron = predicted[0]
      # Inner oracle: Find the best among the golden pronunciations.
      inner_argmin = None
      inner_valmin = _INF
      for golden in golden_lex[word][:golden_cutoff]:
        golden_pron = golden[0]
        ed = edist.EditDistance(golden_pron, predicted_pron,
                                edist.LevenshteinCost)
        val = fdiv(ed.Valmin(), len(golden_pron))  # Per-word phone error.
        if val < inner_valmin:
          inner_argmin = (golden, predicted, ed)
          inner_valmin = val
      assert inner_argmin is not None
      if FLAGS.output_word_stats:
        # Output phone error per predicted pronunciation.
        writer.write("#PRON_PhE\t%s\t%s\t%s\t%f\t%s\n" %
                     (word,
                      ' '.join(inner_argmin[0][0]),
                      ' '.join(predicted_pron),
                      inner_valmin,
                      '\t'.join(predicted[2])))
      argvals.append((inner_argmin, inner_valmin))
    assert argvals != []
    argvals.sort(lambda x, y: cmp(x[1], y[1]))  # Sort by increasing values.
    # Outer oracle: Find the best/etc. among the predicted pronunciations.
    if FLAGS.predicted_oracle_type == "best":
      index = 0
    elif FLAGS.predicted_oracle_type == "worst":
      index = -1
    elif FLAGS.predicted_oracle_type == "median":
      index = len(argvals) >> 1
    else:
      assert False, "This cannot happen."
    ((golden, predicted, ed), pherr) = argvals[index]
    golden_pron = golden[0]
    edits = ed.Valmin()
    assert fdiv(edits, len(golden_pron)) == pherr
    if FLAGS.output_word_stats:
      # Output phone error per word.
      writer.write("#WORD_PhE\t%s\t%s\t%s\t%f\t%s\n" %
                   (word,
                    ' '.join(golden_pron),
                    ' '.join(predicted[0]),
                    pherr,
                    '\t'.join(predicted[2])))
    if edits != 0:
      stats.word_error += 1
    stats.phone_edits += edits
    stats.reference_length += len(golden_pron)
    stats.phone_error += pherr
    stats.squared_phone_error += pherr * pherr
    if pherr > stats.max_phone_error:
      stats.max_phone_error = pherr
    if FLAGS.output_edit_stats:
      alignment = ed.Argmin("<eps>", "<eps>")
      for edit in alignment:
        if edit not in stats.edit_stats:
          stats.edit_stats[edit] = 0
        stats.edit_stats[edit] += 1

    # Compute stats for mean reciprocal rank:
    rr = 0
    found = False
    for i, predicted in enumerate(prons[:predicted_cutoff]):
      predicted_pron = predicted[0]
      # Inner oracle: Find the best among the golden pronunciations.
      for golden in golden_lex[word][:golden_cutoff]:
        golden_pron = golden[0]
        if predicted_pron == golden_pron:
          rr = fdiv(1, 1 + i)  # Reciprocal rank, one-based.
          found = True
          break
      if found:
        break
    assert found == (rr != 0)
    stats.reciprocal_rank += rr
    if FLAGS.output_word_stats:
      writer.write("#WORD_RR\t%s\t%f\n" % (word, rr))

  if FLAGS.output_edit_stats:
    for (i, o), c in stats.edit_stats.iteritems():
      writer.write("#EDIT\t%s\t%s\t%d\n" % (i, o, c))

  if FLAGS.output_summary_stats:
    writer.write("#MISSING: %d words without reference pronunciation\n"
                 % stats.not_found)
    writer.write("#TOTAL:   %d words with reference pronunciation\n"
                 % stats.words_total)
    writer.write("#PRONS:   %d pronunciations\n" % stats.prons)
    writer.write("#PPW:     %.6f pronunciations per word (average)\n"
                 % fdiv(stats.prons, stats.words_total))
    writer.write("#ORACLE:  %s\n" % FLAGS.predicted_oracle_type)
    writer.write("#WER:     %.4f %% word error rate\n"
                 % (100 * fdiv(stats.word_error, stats.words_total)))
    writer.write("#PhER:    %.4f %% phone error rate\n"
                 % (100 * fdiv(stats.phone_edits, stats.reference_length)))
    writer.write("#AVG_PhE: %.4f %% mean phone error per word\n"
                 % (100 * fdiv(stats.phone_error, stats.words_total)))
    writer.write("#RMS_PhE: %.4f %% root mean square phone error per word\n"
                 % (100 * math.sqrt(fdiv(stats.squared_phone_error,
                                          stats.words_total))))
    writer.write("#MAX_PhE: %.4f %% maximum phone error per word\n"
                 % (100.0 * stats.max_phone_error))
    writer.write("#MRR:     %.6f mean reciprocal rank\n"
                 % fdiv(stats.reciprocal_rank, stats.words_total))
  return


def main(argv):
  if len(argv) < 2:
    _stderr.write("Not enough arguments. Use --help for usage information.\n")
    sys.exit(1)

  reader = codecs.open(argv[1], 'r', 'utf_8')
  golden = ReadLexicon(reader)
  reader.close()

  if len(argv) == 3:
    reader = codecs.open(argv[2], 'r', 'utf_8')
  else:
    reader = _stdin
  predicted = ReadLexicon(reader)
  reader.close()

  stats = CompareLexica(golden, predicted, _stdout)  # Uses oracle.
  # Alternatively, 
  #   stats = CompareLexica(golden, predicted, _stdout, 1, 1)
  # would compute word/phone error based on the top golden and top
  # predicted pronunciation.
  return


if __name__ == '__main__':
  import optparse
  usage = "Usage: %prog [options] GOLDEN_LEXICON [PREDICTIONS]"
  parser = optparse.OptionParser(usage)
  parser.add_option("--output_edit_stats", action="store_true",
                    dest="output_edit_stats",
                    help="Output statistics about edit operations.")
  parser.add_option("--output_word_stats", action="store_true",
                    dest="output_word_stats",
                    help="Output phone error and MRR for each word.")
  parser.add_option("--predicted_oracle_type", action="store",
                    dest="predicted_oracle_type",
                    help='What type of oracle to use to choose among multiple predicted pronunciations for a word; valid values are "best" to choose the best pronunciation; "worst" to choose the worst pronunciation; and "median" to choose a pronunciation half-way between best and worst.')
  options, args = parser.parse_args()
  if options.output_edit_stats:
    FLAGS.output_edit_stats = True
  if options.output_word_stats:
    FLAGS.output_word_stats = True
  if options.predicted_oracle_type is None:
    # Don't change the default value.
    pass
  elif options.predicted_oracle_type in ("best", "worst", "median"):
    FLAGS.predicted_oracle_type = options.predicted_oracle_type
  else:
    _stderr.write("Illegal option value --predicted_oracle_type=%s.\n"
                  % options.predicted_oracle_type)
    sys.exit(1)
  main([parser.prog] + args)
