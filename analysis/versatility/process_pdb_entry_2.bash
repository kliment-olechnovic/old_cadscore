#!/bin/bash

OUTPUT_DIRECTORY=$1
PDB_FILE_ZIPPED=$2

cd $(dirname "$0")

QTFIER="QTFier64"

if [ -z "$PDB_FILE_ZIPPED" ] || [ -z "$OUTPUT_DIRECTORY" ]
then
  echo "Fatal error: parameters not provided" 1>&2
  exit 1
fi

PDB_FILE_BASENAME=$(basename $PDB_FILE_ZIPPED .ent.gz)

TMP_DIR=$(mktemp -d)

ATOMS_FILE="$TMP_DIR/input.pdb"
CALC_RESULTS="$TMP_DIR/input.a.qtf"
CALC_LOG="$TMP_DIR/calc.log"
CALC_ERR="$TMP_DIR/calc.err"
TIME_LOG="$TMP_DIR/time.log"
OUTPUT_LOG="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.qtfier.log"
OUTPUT_ERR="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.qtfier.err"

zcat "$PDB_FILE_ZIPPED" | awk '/^END/{exit}1' | egrep '^ATOM' > $ATOMS_FILE

( time -p ($QTFIER $ATOMS_FILE $TMP_DIR/ &> $CALC_ERR) ) 2> $TIME_LOG

cat $CALC_RESULTS | egrep '^MODEL' | awk '{print "spheres " $3}' > $CALC_LOG
cat $CALC_RESULTS | egrep '^MODEL' | awk '{print "vertices " $4}' >> $CALC_LOG

if [ -s $CALC_LOG ]
then
  echo "input $PDB_FILE_BASENAME" > $OUTPUT_LOG
  cat $CALC_LOG $TIME_LOG | sed 's/^/qtfier_/' >> $OUTPUT_LOG
else
  mv $CALC_ERR $OUTPUT_ERR
fi

rm -r "$TMP_DIR"
