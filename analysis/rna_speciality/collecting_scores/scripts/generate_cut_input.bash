#!/bin/bash

INPUTFILE=$1
DATADIR="./cut_input"

mkdir -p "$DATADIR/target"
mkdir -p "$DATADIR/model"

STRIPPEDFILE="$DATADIR/target/full.pdb"

cat $INPUTFILE | egrep '^ATOM|^HETATM' > $STRIPPEDFILE

LINES_COUNT=$(cat $STRIPPEDFILE | wc -l)

for i in $(seq 100 500 $LINES_COUNT)
do
  cat $STRIPPEDFILE | head -n $i > "$DATADIR/model/$i.pdb"
done
