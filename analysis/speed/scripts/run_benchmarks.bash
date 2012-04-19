#!/bin/bash

PDB_DIR=$1

SCRIPT_DIRECTORY=$(dirname $0)

BENCHMARK_CALC_SCRIPT="$SCRIPT_DIRECTORY/Benchmark_calc.bash"
if [ ! -f "$BENCHMARK_CALC_SCRIPT" ]
then
  echo "Fatal error: '$BENCHMARK_CALC_SCRIPT' executable not found" 1>&2
  exit 1
fi

INPUT_IDS_LIST_FILE="$SCRIPT_DIRECTORY/../input/ids"
if [ ! -f "$INPUT_IDS_LIST_FILE" ]
then
  echo "Fatal error: '$INPUT_IDS_LIST_FILE' file not found" 1>&2
  exit 1
fi

OUTPUT_DIR="$SCRIPT_DIRECTORY/../output"
if [ ! -d "$OUTPUT_DIR" ]
then
  echo "Fatal error: '$OUTPUT_DIR' directory not found" 1>&2
  exit 1
fi

cat $INPUT_IDS_LIST_FILE | while read PDB_ID
do
  INPUT_FILE="$PDB_DIR/pdb$PDB_ID.ent"
  if [ -f "$INPUT_FILE" ]
  then
    OUTPUT_FILE="$OUTPUT_DIR/$PDB_ID"
    timeout 300s $BENCHMARK_CALC_SCRIPT -f "$INPUT_FILE" > "$OUTPUT_FILE"
  fi
done
