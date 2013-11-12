#!/bin/bash

RNADIR=$1
INPUT=$2
OUTPUTDIR=$3

cat $INPUT | grep -v pdb_id | egrep -v '\.[A-Z]' | awk '{print $2}' | sed 's/\(....\)_\(.\)\(.*\)-\(.\)\(.*\)/directories\1.pdb \2 \3 \4 \5/' | sed "s|directories|$OUTPUTDIR $RNADIR/|"
