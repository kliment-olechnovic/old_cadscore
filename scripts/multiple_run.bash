#!/bin/bash

INPUT_PATH=$1
CONTACTS_OUTPUT_PREFIX=$2
LOG_OUTPUT_PREFIX=$3

for f in `find $INPUT_PATH -name "*_1-*" -type f`
do
# echo "Processing $f"
  ./single_run.bash $f $CONTACTS_OUTPUT_PREFIX $LOG_OUTPUT_PREFIX
done
