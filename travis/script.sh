#! /bin/bash

set -o nounset

PYTHON_EXECUTABLE="${PYTHON_EXECUTABLE:-$(which python)}"

BAZEL_EXECUTABLE="${BAZEL_EXECUTABLE:-$(which bazel)}"

# Not having a host configuration which is distinct from the target
# configuration means that portions of tools (e.g. protobuf) that are shared
# between build time (protoc, compiled for host) and runtime (protobuf runtime
# library, compiled for target) only need to be built once. For a one-shot
# continuous integration build, this saves a little bit of total build time.
BAZEL_STRATEGY='--nodistinct_host_configuration'

BAZEL_PYTHON="$("$BAZEL_EXECUTABLE" canonicalize-flags -- \
  --python_path="$PYTHON_EXECUTABLE" 2>/dev/null)"

# As of this writing (Bazel 0.8.0 was released a few days ago), Bazel's Android
# tools (@bazel_tools//tools/android/...) do not work with Python 3, so we build
# Android targets in //Mymr/... without setting --python_path (configured by
# $BAZEL_PYTHON).

set -o errexit
set -o xtrace
"$BAZEL_EXECUTABLE" run   $BAZEL_STRATEGY $BAZEL_PYTHON //utils:python_version
"$BAZEL_EXECUTABLE" build $BAZEL_STRATEGY                         //Mymr/...
"$BAZEL_EXECUTABLE" test  $BAZEL_STRATEGY $BAZEL_PYTHON -- //... -//Mymr/...
