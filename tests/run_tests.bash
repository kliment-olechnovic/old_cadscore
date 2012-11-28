#!/bin/bash

set -e

cd $(dirname "$0")

../package.bash cadscore_package
mv ../cadscore_package.tar.gz ./cadscore_package.tar.gz
tar -xf ./cadscore_package.tar.gz
rm ./cadscore_package.tar.gz

CADSCORE_BIN_DIR_ABSOLUTE_PATH="$(pwd)/cadscore_package/bin/"

./basic/run_tests.bash $CADSCORE_BIN_DIR_ABSOLUTE_PATH &
./rna/run_tests.bash $CADSCORE_BIN_DIR_ABSOLUTE_PATH &
wait

rm -r ./cadscore_package
