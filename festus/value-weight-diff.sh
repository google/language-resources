#! /bin/bash

cat value-weight-singleton.h |
sed 's/Singleton/Static/g' |
sed 's/SINGLETON/STATIC/g' |
sed 's/-singleton/-static/g' |
sed 's/Semiring()\./SemiringType::/g' |
git diff --no-index -- value-weight-static.h -
