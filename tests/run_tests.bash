#!/bin/bash

RECOMPILE=false
if [ "$1" == "recompile" ]
then
  RECOMPILE=true
fi

set -e

cd $(dirname "$0")

../package.bash cadscore_package
mv ../cadscore_package.tar.gz ./cadscore_package.tar.gz
tar -xf ./cadscore_package.tar.gz
rm ./cadscore_package.tar.gz

CADSCORE_DIR_ABSOLUTE_PATH="$(pwd)/cadscore_package/"
CADSCORE_BIN_DIR_ABSOLUTE_PATH="$CADSCORE_DIR_ABSOLUTE_PATH/bin/"
CADSCORE_SRC_DIR_ABSOLUTE_PATH="$CADSCORE_DIR_ABSOLUTE_PATH/src/"

if $RECOMPILE
then
  g++ -O3 -o $CADSCORE_BIN_DIR_ABSOLUTE_PATH/voroprot2 $CADSCORE_SRC_DIR_ABSOLUTE_PATH/*.cpp
fi

./basic/run_tests.bash $CADSCORE_BIN_DIR_ABSOLUTE_PATH &
./rna/run_tests.bash $CADSCORE_BIN_DIR_ABSOLUTE_PATH &
./seqalign/run_tests.bash $CADSCORE_BIN_DIR_ABSOLUTE_PATH &
wait

rm -r ./cadscore_package

git status -s ./
