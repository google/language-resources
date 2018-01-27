#! /bin/bash

BAZEL_EXECUTABLE="${BAZEL_EXECUTABLE:-$(which bazel)}"
if [ -z "$BAZEL_EXECUTABLE" ]; then
  echo 'No bazel executable found or configured.'
  exit 1
fi

STRATEGY=''
STARTUP=''
if [ -n "$TRAVIS" ]; then
  STARTUP+=' --batch'
  STRATEGY+=' --jobs=2'
  STRATEGY+=' --noshow_progress'
fi

SHOW='--nocache_test_results --test_output=all'

set -o errexit
set -o xtrace
"$BAZEL_EXECUTABLE" info release
"$BAZEL_EXECUTABLE" run  $STRATEGY       //utils:python_version
"$BAZEL_EXECUTABLE" test $STRATEGY $SHOW //utils:python_version{,_sh}_test
"$BAZEL_EXECUTABLE" $STARTUP test $STRATEGY //...

bazel-bin/setup install
cd fonbund
echo ɡᶣ |
  python example/convert_segments.py \
    config/segment_repository_config_panphon.textproto \
    testdata/tiny_panphon_example.csv |
  fgrep 'cons:+'
