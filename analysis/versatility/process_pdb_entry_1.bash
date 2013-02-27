#!/bin/bash

OUTPUT_DIRECTORY=$1
PDB_FILE_ZIPPED=$2

cd $(dirname "$0")

VOROPROT="../../Release/voroprot2"

if [ -z "$PDB_FILE_ZIPPED" ] || [ -z "$OUTPUT_DIRECTORY" ]
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
OUTPUT_LOG="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.voroprot2.log"
OUTPUT_ERR="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.voroprot2.err"

zcat "$PDB_FILE_ZIPPED" | awk '/^END/{exit}1' | egrep '^ATOM' | $VOROPROT --mode collect-atoms --radius-classes "resources/vdwr_classes" --radius-members "resources/vdwr_members" > $ATOMS_FILE

( time -p (cat $ATOMS_FILE | $VOROPROT --mode calc-quadruples --monitoring-level 9 --clog-file $CALC_LOG > /dev/null 2> $CALC_ERR) ) 2> $TIME_LOG

echo "input $PDB_FILE_BASENAME" > $OUTPUT_LOG
cat $CALC_LOG $TIME_LOG | sed 's/^/voroprot2_/' >> $OUTPUT_LOG

if [ -s $CALC_ERR ]
then
  mv $CALC_ERR $OUTPUT_ERR
fi

rm -r "$TMP_DIR"
