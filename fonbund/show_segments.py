# Copyright 2018 Google LLC. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Display raw segment inventories.
"""

from absl import app
from absl import flags
from absl import logging
from fonbund import segments

FLAGS = flags.FLAGS

flags.DEFINE_string('db', '', 'Segment inventory, e.g.'
                    ' "panphon", "phoible", "phoible_fonetikode"')


def main(unused_argv):
  if not FLAGS.db:
    app.usage(shorthelp=True, exitcode=2)
  if FLAGS.db not in segments.TABLES:
    logging.fatal('--db="%s" not recognized; possible values are %s',
                  FLAGS.db, ', '.join('"%s"' % t for t in segments.TABLES))
  for row in segments.SelectFrom(FLAGS.db):
    print('%s\t%s' % (row[0], ' '.join(row[1:])))
  return


if __name__ == '__main__':
  app.run(main)
