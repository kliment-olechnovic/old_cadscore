#!/bin/bash

OUTPUT_DIRECTORY=$1
PDB_FILE_ZIPPED=$2

cd $(dirname "$0")

VOROPROT="./voroprot2"
AWVORONOI_DIR="./awvoronoi"

if [ -z "$PDB_FILE_ZIPPED" ] || [ -z "$OUTPUT_DIRECTORY" ]
then
  echo "Fatal error: parameters not provided" 1>&2
  exit 1
fi

PDB_FILE_BASENAME=$(basename $PDB_FILE_ZIPPED .ent.gz)

TMP_DIR=$(mktemp -d)

ATOMS_FILE="$TMP_DIR/input.atoms"
CALC_RESULTS="$TMP_DIR/results.xml"
CALC_LOG="$TMP_DIR/calc.log"
CALC_ERR="$TMP_DIR/calc.err"
TIME_LOG="$TMP_DIR/time.log"
OUTPUT_LOG="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.awvoronoi.log"
OUTPUT_ERR="$OUTPUT_DIRECTORY/$PDB_FILE_BASENAME.awvoronoi.err"

zcat "$PDB_FILE_ZIPPED" | awk '/^END/{exit}1' | egrep '^ATOM' | $VOROPROT --mode collect-atoms --radius-classes "resources/vdwr_classes" --radius-members "resources/vdwr_members" --include-insertions | tail -n+3 | awk '{print $6 " " $7 " " $8 " " $9}' > $ATOMS_FILE

cd $AWVORONOI_DIR

( time -p (timeout 1200 java -Xms1246m -jar awvoronoi-run-1.0.0.jar $ATOMS_FILE $CALC_RESULTS &> $CALC_ERR) ) 2> $TIME_LOG

cat $CALC_RESULTS | grep '<vertex id' | grep -v 'INF' | wc -l | sed 's/^/atoms /' > $CALC_LOG
cat $CALC_RESULTS | grep '<cell id' | grep -v 'INF' | wc -l | sed 's/^/vertices /' >> $CALC_LOG

echo "input $PDB_FILE_BASENAME" > $OUTPUT_LOG
cat $CALC_LOG $TIME_LOG | sed 's/^/awvoronoi_/' >> $OUTPUT_LOG

rm -r "$TMP_DIR"
