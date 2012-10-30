#!/bin/bash

VOROPROT=$1
INPUT=$2
AREA_TYPE=$3

(cat $INPUT | $VOROPROT --mode collect-atoms ; cat $INPUT | $VOROPROT --mode collect-atoms | $VOROPROT --mode "calc-inter-atom-$AREA_TYPE" | $VOROPROT --mode calc-inter-residue-contacts) | $VOROPROT --mode print-stacking-nucleotides-contacts --prefix $(basename $INPUT)
