#!/bin/bash

VOROPROT2="../Release/voroprot2"

INPUTFILE=$1

OUTPUTDIR="./parametrization/"

rm -r $OUTPUTDIR
mkdir -p $OUTPUTDIR

RADIUS=4.2
LOWCOUNT=50
ASPOINTS=0
SD3=1
LCFILE=$OUTPUTDIR/lowcounts.list
for i in {1..200..5}
do
  LOWCOUNT=$i
  echo "lowcount $LOWCOUNT" >> $LCFILE
  (time -p cat $INPUTFILE | $VOROPROT2 --mode collect-atoms --radius-classes ../resources/vdwr_classes_simple.txt --radius-members ../resources/vdwr_members_simple.txt --include-heteroatoms 1 --include-water 0 | $VOROPROT2 --mode construct-apollonius-quadrupalization --radius $RADIUS --low-count $LOWCOUNT --as-points $ASPOINTS --search-for-d3 $SD3 --check-for-orphans) > /dev/null 2>> $LCFILE
done

RADIUS=4.2
LOWCOUNT=50
ASPOINTS=0
SD3=1
RFILE=$OUTPUTDIR/radii.list
for i in {20..99..1}
do
  RADIUS=`echo "10*0.$i" | bc`
  echo "radius $RADIUS" >> $RFILE
  (time -p cat $INPUTFILE | $VOROPROT2 --mode collect-atoms --radius-classes ../resources/vdwr_classes_simple.txt --radius-members ../resources/vdwr_members_simple.txt --include-heteroatoms 1 --include-water 0 | $VOROPROT2 --mode construct-apollonius-quadrupalization --radius $RADIUS --low-count $LOWCOUNT --as-points $ASPOINTS --search-for-d3 $SD3 --check-for-orphans) > /dev/null 2>> $RFILE
done
