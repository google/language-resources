#! /bin/bash

set -o errexit
set -o nounset
set -o pipefail

LANG=en

# Tools:
runfiles="${0}.runfiles"
sh_utter="$runfiles/language_resources/utils/sh-utter"
prefix=`pwd`/"$runfiles"/language_resources/"$LANG"/sparrowhawk/
config=sparrowhawk_configuration.ascii_proto

"$sh_utter" --config="$config"  --prefix="$prefix"

