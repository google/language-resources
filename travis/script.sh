#! /bin/bash

if [ -z "$ANDROID_HOME" ]; then
  echo "Building this project with Bazel requires ANDROID_HOME to be set."
  exit 1
fi

BAZEL_EXECUTABLE="${BAZEL_EXECUTABLE:-$(which bazel)}"

set -o nounset

# Not having a host configuration which is distinct from the target
# configuration means that portions of tools (e.g. protobuf) that are shared
# between build time (protoc, compiled for host) and runtime (protobuf runtime
# library, compiled for target) only need to be built once. For a one-shot
# continuous integration build, this saves a little bit of total build time.
STRATEGY='--nodistinct_host_configuration'
STRATEGY+=' --test_timeout_filters=-long'
STRATEGY+=' --jobs=2'

SHOW='--nocache_test_results --test_output=all'

# As of this writing (Bazel 0.8.0 was released a few days ago), Bazel's Android
# tools (@bazel_tools//tools/android/...) do not work with Python 3, so we build
# Android targets in //Mymr/... without setting --python_path (configured by
# $BAZEL_PYTHON).

set -o errexit
set -o xtrace
"$BAZEL_EXECUTABLE" info release
"$BAZEL_EXECUTABLE" run   $STRATEGY       //utils:python_version
"$BAZEL_EXECUTABLE" test  $STRATEGY $SHOW //utils:python_version_test
"$BAZEL_EXECUTABLE" test  $STRATEGY       //...
