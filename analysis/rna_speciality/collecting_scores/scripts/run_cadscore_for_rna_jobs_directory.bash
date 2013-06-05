#!/bin/bash

CADSCORE_BIN_DIR=$1
JOBS_DIR=$2
OUTPUT_DIR=$3

if [ -z "$CADSCORE_BIN_DIR" ] || [ -z "$JOBS_DIR" ] || [ -z "$OUTPUT_DIR" ]
then
  echo "Fatal error: parameters not provided" 1>&2
  exit 1
fi

TMP_DIR=$(mktemp -d)

LOGS_FILE="$OUTPUT_DIR/logs"
true > "$LOGS_FILE"

find $JOBS_DIR -mindepth 1 -maxdepth 1 -type d  | while read JOB_DIR
do
  find "$JOB_DIR/target" -type f | while read TARGET_FILE
  do
    find "$JOB_DIR/model" -type f | while read MODEL_FILE
    do
      MODEL_BASENAME=$(basename $MODEL_FILE)
      RENUMBERED_MODEL_FILE="$TMP_DIR/$MODEL_BASENAME"
      TEMP_LOGS_FILE="$TMP_DIR/$MODEL_BASENAME.logs"
      true > "$TEMP_LOGS_FILE"
      (cat $TARGET_FILE | $CADSCORE_BIN_DIR/voroprot2 --mode collect-atoms ; cat $MODEL_FILE | $CADSCORE_BIN_DIR/voroprot2 --mode collect-atoms) | $CADSCORE_BIN_DIR/voroprot2 --mode x-renumber-residues-by-reference --print-summary-log --output-in-pdb-format > $RENUMBERED_MODEL_FILE 2>> $TEMP_LOGS_FILE
      $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/db -t $TARGET_FILE -m $RENUMBERED_MODEL_FILE -a -n -u 2>> $TEMP_LOGS_FILE
      if [ -s "$TEMP_LOGS_FILE" ]
      then
        echo "Log for target $(basename $TARGET_FILE) model $(basename $MODEL_FILE):" &>> $LOGS_FILE
        cat "$TEMP_LOGS_FILE" | sed 's/^/\t/' &>> $LOGS_FILE
      fi
    done
  done
done

rm -r $TMP_DIR

$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/db | sort -r | column -t > $OUTPUT_DIR/scores
