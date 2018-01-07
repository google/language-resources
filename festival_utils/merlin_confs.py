#! /usr/bin/env python
#
# Copyright 2016 Google Inc. All Rights Reserved.
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

import ConfigParser
import sys

if __name__ == "__main__":

  conf = sys.argv[1]
  count = int(sys.argv[2])

  count -= 4
  train = count / 2
  valid = count / 4
  test = count / 4

  Config = ConfigParser.ConfigParser()
  Config.read(conf)

  # Setup data splits.
  splits = [["train_file_number", 2], ["test_file_number", 4],
            ["valid_file_number", 4]]

  map(lambda x: Config.set("Data", x[0], count / x[1]), splits)

  # Setup DNN architecture.
  Config.set("Architecture", "training_epochs", 2)
  Config.set("Architecture", "warmup_epoch", 2)

  Config.write(sys.stdout)
