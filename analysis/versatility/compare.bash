#!/bin/bash

VOROPROT="./voroprot2"
QTFIER="./QTFier32"

OUTPUT_DIRECTORY=$1
PDB_FILE_UNZIPPED=$2
AWVORONOI_DIR=$3

cd $(dirname "$0")

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
QTFIER_CALC_TIME="$QTFIER_CALC_RESULTS.time"

VOROPROT_INPUT_FILE="$TMP_DIR/input.balls"
VOROPROT_CALC_RESULTS="$TMP_DIR/vertices"
VOROPROT_CALC_QUADRUPLES="$VOROPROT_CALC_RESULTS.quadruples"
VOROPROT_CALC_LOG="$VOROPROT_CALC_RESULTS.log"
VOROPROT_CALC_TIME="$VOROPROT_CALC_RESULTS.time"

AWVORONOI_INPUT_FILE=$VOROPROT_INPUT_FILE
AWVORONOI_CALC_RESULTS="$TMP_DIR/awvoronoi_result.xml"
AWVORONOI_CALC_QUADRUPLES="$AWVORONOI_CALC_RESULTS.quadruples"
AWVORONOI_CALC_LOG="$AWVORONOI_CALC_RESULTS.log"
AWVORONOI_CALC_TIME="$AWVORONOI_CALC_RESULTS.time"

OUTPUT_RESULTS="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.results"

cat "$PDB_FILE_UNZIPPED" | awk '/^END/{exit}1' | egrep '^ATOM' | egrep -v '.{76} H' > $QTFIER_INPUT_FILE

( time -p (timeout 600 $QTFIER $QTFIER_INPUT_FILE $TMP_DIR/ &> /dev/null) ) 2> $QTFIER_CALC_TIME
cat $QTFIER_CALC_RESULTS | egrep '^QTCELL' | awk '{print ($3-1) " " ($4-1) " " ($5-1) " " ($6-1)}' | awk '{split($0,array," "); asort(array); printf array[1] " " array[2] " " array[3] " " array[4] "\n"}' | sort > $QTFIER_CALC_QUADRUPLES
cat $QTFIER_CALC_RESULTS | egrep '^MODEL' | awk '{print "qtfier_atoms " $3}' > $QTFIER_CALC_LOG
cat $QTFIER_CALC_RESULTS | egrep '^MODEL' | awk '{print "qtfier_vertices " $4}' >> $QTFIER_CALC_LOG

if [ -s $QTFIER_CALC_LOG ]
then
  cat $QTFIER_CALC_RESULTS | egrep '^QTVTX' | awk '{print $4 " " $5 " " $6 " " $7}' > $VOROPROT_INPUT_FILE
  ( time -p (cat $VOROPROT_INPUT_FILE | timeout 600 $VOROPROT --mode x-calc-quadruples-2 --augment --print-log --clog-file $VOROPROT_CALC_LOG > $VOROPROT_CALC_RESULTS) ) 2> $VOROPROT_CALC_TIME
  cat $VOROPROT_CALC_RESULTS | awk '{print $1 " " $2 " " $3 " " $4}' | awk '{split($0,array," "); asort(array); printf array[1] " " array[2] " " array[3] " " array[4] "\n"}' | sort > $VOROPROT_CALC_QUADRUPLES
  
  echo "input $PDB_FILE_BASENAME" > $OUTPUT_RESULTS
  cat $VOROPROT_CALC_LOG >> $OUTPUT_RESULTS
  cat $VOROPROT_CALC_TIME >> $OUTPUT_RESULTS
  
  cat $QTFIER_CALC_LOG >> $OUTPUT_RESULTS
  cat $QTFIER_CALC_TIME | sed 's/^/qtfier_/' >> $OUTPUT_RESULTS
  time -p ( cat $VOROPROT_INPUT_FILE | $VOROPROT --mode x-compare-two-sets-of-quadruples-2 --file1 $QTFIER_CALC_QUADRUPLES --file2 $VOROPROT_CALC_QUADRUPLES | sed 's/^/qtfier_/' >> $OUTPUT_RESULTS )
  
  if [ -n "$AWVORONOI_DIR" ]
  then
    cd $AWVORONOI_DIR
    ( time -p (timeout 1200 java -Xms1246m -jar awvoronoi-run-1.0.0.jar $AWVORONOI_INPUT_FILE $AWVORONOI_CALC_RESULTS &> /dev/null) ) 2> $AWVORONOI_CALC_TIME
    cd ..
    cat $AWVORONOI_CALC_RESULTS | grep '<cell id' | grep -v 'INF' | tr '<' ' ' | tr '>' ' ' | awk '{print ($4-1) " " ($5-1) " " ($6-1) " " ($7-1)}' | awk '{split($0,array," "); asort(array); printf array[1] " " array[2] " " array[3] " " array[4] "\n"}' | sort > $AWVORONOI_CALC_QUADRUPLES
    cat $AWVORONOI_CALC_RESULTS | grep '<vertex id' | grep -v 'INF' | wc -l | sed 's/^/awvoronoi_atoms /' > $AWVORONOI_CALC_LOG
    cat $AWVORONOI_CALC_RESULTS | grep '<cell id' | grep -v 'INF' | wc -l | sed 's/^/awvoronoi_vertices /' >> $AWVORONOI_CALC_LOG
    
    cat $AWVORONOI_CALC_LOG >> $OUTPUT_RESULTS
    cat $AWVORONOI_CALC_TIME | sed 's/^/awvoronoi_/' >> $OUTPUT_RESULTS
    time -p ( cat $VOROPROT_INPUT_FILE | $VOROPROT --mode x-compare-two-sets-of-quadruples-2 --file1 $AWVORONOI_CALC_QUADRUPLES --file2 $VOROPROT_CALC_QUADRUPLES | sed 's/^/awvoronoi_/' >> $OUTPUT_RESULTS )
  fi
  
  echo
  cat $OUTPUT_RESULTS
  echo
fi

rm -r "$TMP_DIR"
