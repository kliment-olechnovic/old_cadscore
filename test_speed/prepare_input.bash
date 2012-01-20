#!/bin/bash

INPUTFILE=$1
DATADIR="./data"
STRIPPEDFILE="$DATADIR/full.pdb"

rm -r $DATADIR
mkdir -p $DATADIR

cat $INPUTFILE | egrep '^ATOM|^HETATM' > $STRIPPEDFILE

for i in {50..1500..50}
do
  cat $STRIPPEDFILE | head -n $i > "$DATADIR/$i.pdb"
done

rm $STRIPPEDFILE
