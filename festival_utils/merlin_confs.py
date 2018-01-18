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
"""Updates the given merlin conf using the global config.


Usage

   python festival_utils/merlin_confs.py \
   30 \
   third_party/merlin/acoustic_dnn.conf \
   festival_utils/testdata/merlin_params.json  \
   acoustic
"""
import argparse
import ConfigParser
import json
import sys

parser = argparse.ArgumentParser(
    description="Update given configuration using the global_config.")
parser.add_argument("data_count", help="Total number of data.")
parser.add_argument("conf_file", help="Path to merlin conf file")
parser.add_argument(
    "global_config", help="Path to the global config json file.")
parser.add_argument(
    "conf_type", help="Total number of data.", choices=["acoustic", "duration"])

if __name__ == "__main__":
  opts = parser.parse_args()

  count = int(opts.data_count)
  config = json.load(open(opts.global_config))

  # Remove entry which are not of conf_type.
  settings = filter(lambda x: opts.conf_type == x["key"].split(".")[0], config)
  settings = [
      entry["key"].split(".")[1:] + [entry["value"]] for entry in settings
  ]

  Config = ConfigParser.ConfigParser()
  Config.read(opts.conf_file)

  # Setup data splits.
  splits = [["train_file_number", 2], ["test_file_number", 4],
            ["valid_file_number", 4]]
  map(lambda x: Config.set("Data", x[0], count / x[1]), splits)

  # Update configurations from json.
  for section, option, value in settings:
    Config.set(section, option, value)

  Config.write(sys.stdout)
