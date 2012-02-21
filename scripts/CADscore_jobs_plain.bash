#!/bin/bash

CADSCORE_SCRIPTS_DIR=$(dirname $0)

JOBS_DIR=$1
DATABASE_DIR=$2
CADSCORE_CALC_OPTIONS=$3

if [ -z "$JOBS_DIR" ] || [ ! -d "$JOBS_DIR" ]
then
  echo "Missing existing jobs directory" 1>&2
  exit 1
fi

if [ -z "$DATABASE_DIR" ]
then
  echo "Missing database directory path" 1>&2
  exit 1
fi

find $JOBS_DIR -mindepth 1 -maxdepth 1 -type d | while read CADSCORE_JOB_DIR
do
  find "$CADSCORE_JOB_DIR/target" -type f | while read TARGET_FILE
  do
    find "$CADSCORE_JOB_DIR/model" -type f | while read MODEL_FILE
    do
      $CADSCORE_SCRIPTS_DIR/CADscore_calc.bash -D "$DATABASE_DIR" -t "$TARGET_FILE" -m "$MODEL_FILE" $CADSCORE_CALC_OPTIONS
    done
  done
done

$CADSCORE_SCRIPTS_DIR/CADscore_read_table.bash -D "$DATABASE_DIR" | column -t > "$DATABASE_DIR/table"
