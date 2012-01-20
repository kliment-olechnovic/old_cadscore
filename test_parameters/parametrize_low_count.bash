#!/bin/bash

VOROPROT2="../Release/voroprot2"

IN_FILE=$1

RADIUS="4.2"
LOW_COUNT="50"
AS_POINTS="0"
SEARCH_FOR_D3="1"

for i in {1..100..1}
do
  LOW_COUNT=$i
  echo "param $LOW_COUNT" 1>&2
  time -p cat $IN_FILE | $VOROPROT2 --mode collect-atoms --include-heteroatoms 1 --include-water 0 | $VOROPROT2 --mode construct-apollonius-quadrupalization --radius $RADIUS --low-count $LOW_COUNT --as-points $AS_POINTS --search-for-d3 $SEARCH_FOR_D3
done