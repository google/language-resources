#! /bin/bash

if [ -z "$ANDROID_HOME" ]; then
  echo "Building this project with Bazel requires ANDROID_HOME to be set."
  echo "It should point to the Android SDK, e.g. in $HOME/Android/Sdk"
  exit 1
fi

BAZEL_EXECUTABLE="${BAZEL_EXECUTABLE:-$(which bazel)}"

# Not having a host configuration which is distinct from the target
# configuration means that portions of tools (e.g. protobuf) that are shared
# between build time (protoc, compiled for host) and runtime (protobuf runtime
# library, compiled for target) only need to be built once. For a one-shot
# continuous integration build, this saves a little bit of total build time.
STRATEGY='--nodistinct_host_configuration'
STARTUP=''

if [ -n "$TRAVIS" ]; then
  STARTUP+=' --batch'
  STRATEGY+=' --jobs=2'
  STRATEGY+=' --noshow_progress'
  STRATEGY+=' --test_timeout_filters=-long'
fi

SHOW='--nocache_test_results --test_output=all'

set -o errexit
set -o xtrace
"$BAZEL_EXECUTABLE" info release
"$BAZEL_EXECUTABLE" run  $STRATEGY       //utils:python_version
"$BAZEL_EXECUTABLE" test $STRATEGY $SHOW //utils:python_version{,_sh}_test
"$BAZEL_EXECUTABLE" $STARTUP test $STRATEGY //...
