#!/bin/bash

OUTPUT_DIRECTORY=$1
PDB_FILE_UNZIPPED=$2

cd $(dirname "$0")

VOROPROT="./voroprot2"
QTFIER="./QTFier32"

if [ -z "$PDB_FILE_UNZIPPED" ] || [ -z "$OUTPUT_DIRECTORY" ]
then
  echo "Fatal error: parameters not provided" 1>&2
  exit 1
fi

PDB_FILE_BASENAME=$(basename $PDB_FILE_UNZIPPED .ent)

TMP_DIR=$(mktemp -d)

QTFIER_INPUT_FILE="$TMP_DIR/input.pdb"
QTFIER_CALC_RESULTS="$TMP_DIR/input.a.qtf"
QTFIER_CALC_QUADRUPLES="$QTFIER_CALC_RESULTS.quadruples"
QTFIER_CALC_LOG="$QTFIER_CALC_RESULTS.log"
VOROPROT_INPUT_FILE="$TMP_DIR/input.balls"
VOROPROT_CALC_RESULTS="$TMP_DIR/vertices"
VOROPROT_CALC_QUADRUPLES="$VOROPROT_CALC_RESULTS.quadruples"
VOROPROT_CALC_LOG="$VOROPROT_CALC_RESULTS.log"
OUTPUT_RESULTS="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.results"

cat "$PDB_FILE_UNZIPPED" | awk '/^END/{exit}1' | egrep '^ATOM' > $QTFIER_INPUT_FILE

timeout 1200 $QTFIER $QTFIER_INPUT_FILE $TMP_DIR/
cat $QTFIER_CALC_RESULTS | egrep '^QTCELL' | awk '{print ($3-1) " " ($4-1) " " ($5-1) " " ($6-1)}' | awk '{split($0,array," "); asort(array); printf array[1] " " array[2] " " array[3] " " array[4] "\n"}' | sort > $QTFIER_CALC_QUADRUPLES
cat $QTFIER_CALC_RESULTS | egrep '^MODEL' | awk '{print "qtfier_atoms " $3}' > $QTFIER_CALC_LOG
cat $QTFIER_CALC_RESULTS | egrep '^MODEL' | awk '{print "qtfier_vertices " $4}' >> $QTFIER_CALC_LOG

if [ -s $QTFIER_CALC_LOG ]
then
  cat $QTFIER_CALC_RESULTS | egrep '^QTVTX' | awk '{print $4 " " $5 " " $6 " " $7}' > $VOROPROT_INPUT_FILE
  cat $VOROPROT_INPUT_FILE | $VOROPROT --mode x-calc-quadruples-2 --print-log --clog-file $VOROPROT_CALC_LOG > $VOROPROT_CALC_RESULTS
  cat $VOROPROT_CALC_RESULTS | awk '{print $1 " " $2 " " $3 " " $4}' | awk '{split($0,array," "); asort(array); printf array[1] " " array[2] " " array[3] " " array[4] "\n"}' | sort > $VOROPROT_CALC_QUADRUPLES
  
  echo "input $PDB_FILE_BASENAME" > $OUTPUT_RESULTS
  cat $QTFIER_CALC_LOG $VOROPROT_CALC_LOG >> $OUTPUT_RESULTS
  cat $VOROPROT_INPUT_FILE | $VOROPROT --mode x-compare-two-sets-of-quadruples-2 --file1 $QTFIER_CALC_QUADRUPLES --file2 $VOROPROT_CALC_QUADRUPLES --perturbation 0.001  >> $OUTPUT_RESULTS
  cat $VOROPROT_INPUT_FILE | $VOROPROT --mode x-compare-two-sets-of-quadruples-2 --file1 $QTFIER_CALC_QUADRUPLES --file2 $VOROPROT_CALC_QUADRUPLES --perturbation 0.01 | tail -1  >> $OUTPUT_RESULTS
  cat $VOROPROT_INPUT_FILE | $VOROPROT --mode x-compare-two-sets-of-quadruples-2 --file1 $QTFIER_CALC_QUADRUPLES --file2 $VOROPROT_CALC_QUADRUPLES --perturbation 0.1 | tail -1 >> $OUTPUT_RESULTS
fi

rm -r "$TMP_DIR"

echo
cat $OUTPUT_RESULTS
echo
