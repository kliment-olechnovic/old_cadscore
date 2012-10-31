#!/bin/bash

VOROPROT=$1
INPUT=$2
AREA_TYPE=$3

cat $INPUT | $VOROPROT --mode collect-atoms | $VOROPROT --mode "calc-inter-atom-$AREA_TYPE" | $VOROPROT --mode summarize-inter-atom-contacts | tail -n +3 | sed "s/^/$(basename $INPUT) /"
