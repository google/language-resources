#! /bin/bash

if [ ${RUN_DOCKER} == true ]; then
  echo "Testing"
  ../docker-images/test-merlin/setup_test.sh
fi
