#!/bin/bash

INPUT_PATH=$1
OUTPUT_PREFIX=$2

for f in `find $INPUT_PATH -name "*" -type f`
do
# echo "Processing $f"
  ./single_run.bash $f $OUTPUT_PREFIX
done
