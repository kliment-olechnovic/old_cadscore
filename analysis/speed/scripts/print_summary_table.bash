#!/bin/bash

VALID_HEADER="input voroprot2_atoms voroprot2_vertices voroprot2_time qtfier_atoms qtfier_vertices qtfier_time"

SCRIPT_DIRECTORY=$(dirname $0)

OUTPUT_DIR="$SCRIPT_DIRECTORY/../output"
if [ ! -d "$OUTPUT_DIR" ]
then
  echo "Fatal error: '$OUTPUT_DIR' directory not found" 1>&2
  exit 1
fi

echo $VALID_HEADER

find $OUTPUT_DIR -type f | while read FILE_PATH
do
  FILE_HEADER=$(echo `cat $FILE_PATH | awk '{print $1}'`)
  if [ "$FILE_HEADER" == "$VALID_HEADER" ]
  then
    echo `cat $FILE_PATH | awk '{print $2}'`
  fi
done
