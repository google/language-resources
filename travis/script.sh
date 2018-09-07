#! /bin/bash

if [ -z "$ANDROID_HOME" ]; then
  echo 'Building the Android parts of this project requires ANDROID_HOME'
  echo 'to point to the Android SDK, e.g. in $HOME/Android/Sdk'
fi

BAZEL_EXECUTABLE="${BAZEL_EXECUTABLE:-$(which bazel)}"
if [ -z "$BAZEL_EXECUTABLE" ]; then
  echo 'No bazel executable found or configured.'
  exit 1
fi

STRATEGY='--compilation_mode=opt --verbose_failures'
if [ -n "$TRAVIS" ]; then
  STRATEGY+=' --curses=no'
  STRATEGY+=' --jobs=3'
  STRATEGY+=' --local_resources=2048,.5,1.0'
  STRATEGY+=' --test_timeout_filters=-long'
fi

SHOW='--nocache_test_results --test_output=all'

set -o errexit
set -o xtrace
"$BAZEL_EXECUTABLE" info release
"$BAZEL_EXECUTABLE" run  $STRATEGY       //utils:python_version
"$BAZEL_EXECUTABLE" test $STRATEGY $SHOW //utils:python_version{,_sh}_test

"$BAZEL_EXECUTABLE" query '//... - rdeps(//..., filter("/textnorm/", //...))' |
  xargs "$BAZEL_EXECUTABLE" test $STRATEGY --
