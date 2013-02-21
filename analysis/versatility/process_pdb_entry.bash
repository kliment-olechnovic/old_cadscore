#!/bin/bash

OUTPUT_DIRECTORY=$1
PDB_FILE_ZIPPED=$2

cd $(dirname "$0")

VOROPROT="../../Release/voroprot2"

if [ -z "$VOROPROT" ] || [ -z "$PDB_FILE_ZIPPED" ] || [ -z "$OUTPUT_DIRECTORY" ]
then
  echo "Fatal error: parameters not provided" 1>&2
  exit 1
fi

PDB_FILE_BASENAME=$(basename $PDB_FILE_ZIPPED .ent.gz)

TMP_DIR=$(mktemp -d)

ATOMS_FILE="$TMP_DIR/input.atoms"
CALC_LOG="$TMP_DIR/calc.log"
CALC_ERR="$TMP_DIR/calc.err"
TIME_LOG="$TMP_DIR/time.log"
OUTPUT_LOG="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.log"
OUTPUT_ERR="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.err"

zcat "$PDB_FILE_ZIPPED" | $VOROPROT --mode collect-atoms --radius-classes "resources/vdwr_classes" --radius-members "resources/vdwr_members" > $ATOMS_FILE

( time -p (cat $ATOMS_FILE | $VOROPROT --mode calc-quadruples --monitoring-name $PDB_FILE_BASENAME --monitoring-level 9 --clog-file $CALC_LOG > /dev/null 2> $CALC_ERR) ) 2> $TIME_LOG

cat $CALC_LOG $TIME_LOG > $OUTPUT_LOG

if [ -s $CALC_ERR ]
then
  mv $CALC_ERR $OUTPUT_ERR
fi

rm -r "$TMP_DIR"
