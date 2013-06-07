#!/bin/bash

CADSCORE_BIN_DIR=$1
OUTPUT_DIR=$2
JOBS_DIR=$3

find $JOBS_DIR -mindepth 1 -maxdepth 1 -type d  | sort | while read JOB_DIR
do
  find "$JOB_DIR/target" -type f | while read TARGET_FILE
  do
    find "$JOB_DIR/model" -type f | while read MODEL_FILE
    do
      echo "'$CADSCORE_BIN_DIR' '$OUTPUT_DIR' '$TARGET_FILE' '$MODEL_FILE'"
    done
  done
done
