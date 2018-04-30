#! /bin/bash
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
#
# Copyright 2017 Google, Inc.

set -x
set -eo pipefail

TEST_IMAGE_NAME="test-merlin"
IMAGE_ID="$(docker images -q ${TEST_IMAGE_NAME})"
CONTAINER_NAME=merlin-test-box

# Start new docker test box.
if [[ $(docker ps -q --filter "name=${CONTAINER_NAME}" | wc -l) == 1 ]]; then
  docker stop "${CONTAINER_NAME}"
  docker rm "${CONTAINER_NAME}"
fi

# Build the test image if not available.
if [[ $IMAGE_ID == "" ]]; then
  docker build -t "${TEST_IMAGE_NAME}" .
  IMAGE_ID="$(docker images -q ${TEST_IMAGE_NAME})"
fi

# Create a container.
docker run --name "${CONTAINER_NAME}" -dt "${IMAGE_ID}"  /bin/bash

CONTAINER_ID=$(docker ps -aqf "name=${CONTAINER_NAME}")

REMOTE_TEST_DIR="${CONTAINER_ID}:/usr/local/src/"

# Copy required resources.
docker cp ../merlin/entry.sh "${REMOTE_TEST_DIR}/entry.sh"
docker cp tests.sh "${REMOTE_TEST_DIR}/tests.sh"

for data in testdata/*.txt; do
  basename=$( basename "${data}" )
  docker cp "${data}" "${REMOTE_TEST_DIR}/${basename}"
done

# Run merlin setup on the test data.
docker exec "${CONTAINER_ID}" bash /usr/local/src/entry.sh

# Run tests.
docker exec "${CONTAINER_ID}" bash /usr/local/src/tests.sh
