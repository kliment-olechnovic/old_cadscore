#!/bin/bash

VOROPROT="./voroprot2"

INPUT=$1

(cat $INPUT | $VOROPROT --mode collect-atoms ; cat $INPUT | $VOROPROT --mode collect-atoms | $VOROPROT --mode calc-inter-atom-contacts | $VOROPROT --mode calc-inter-residue-contacts) \
 | $VOROPROT --mode categorize-inter-nucleotide-side-chain-contacts --diagnostic-output --output-normals-cos --output-rings-shift | sed "s/^/$(basename $INPUT) /"
