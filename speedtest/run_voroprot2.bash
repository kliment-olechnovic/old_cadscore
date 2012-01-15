#!/bin/bash

VOROPROT2="../Release/voroprot2"

INPUTFILE=$1
RADIUS=4.2
LOWCOUNT=50
ASPOINTS=0
SD3=1

while getopts “p:s:f:” OPTION
do
  case $OPTION in
    p)
      ASPOINTS=$OPTARG
      ;;
    s)
      SD3=$OPTARG
      ;;
    f)
      INPUTFILE=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

OUTPUTFILE="$INPUTFILE.vq.p$ASPOINTS.s$SD3.out"
OUTPUTFILELOG="$OUTPUTFILE.log"

(echo "input $INPUTFILE" | sed 's/\.*\S*\/\(\S*\).pdb/\1/' >&2 ; time -p cat $INPUTFILE | $VOROPROT2 --mode collect-atoms --radius-classes ../resources/vdwr_classes_simple.txt --radius-members ../resources/vdwr_members_simple.txt --include-heteroatoms 1 --include-water 0 | $VOROPROT2 --mode construct-apollonius-quadrupalization --radius $RADIUS --low-count $LOWCOUNT --as-points $ASPOINTS --search-for-d3 $SD3 --check-for-orphans) > $OUTPUTFILE 2> $OUTPUTFILELOG
for i in 1 2 ; do cat $OUTPUTFILELOG | cut --delimiter " " --fields $i | paste -s ; done > $OUTPUTFILELOG.table
