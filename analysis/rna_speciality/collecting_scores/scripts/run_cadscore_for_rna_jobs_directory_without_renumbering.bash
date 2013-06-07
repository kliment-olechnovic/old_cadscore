#!/bin/bash

CADSCORE_BIN_DIR=$1
JOBS_DIR=$2
OUTPUT_DIR=$3

if [ -z "$CADSCORE_BIN_DIR" ] || [ -z "$JOBS_DIR" ] || [ -z "$OUTPUT_DIR" ]
then
  echo "Fatal error: parameters not provided" 1>&2
  exit 1
fi

find $JOBS_DIR -mindepth 1 -maxdepth 1 -type d  | sort | while read JOB_DIR
do
  find "$JOB_DIR/target" -type f | while read TARGET_FILE
  do
    find "$JOB_DIR/model" -type f | while read MODEL_FILE
    do
      echo "Processing target $(basename $TARGET_FILE) model $(basename $MODEL_FILE):"
      $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/db -t $TARGET_FILE -m $MODEL_FILE -a -n -u -q -e "rnadi.bash" -y -g -x
      echo ""
    done
  done
done

$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/db | sed 's/.pdb//g' | sort -r | column -t > $OUTPUT_DIR/calculated_scores
