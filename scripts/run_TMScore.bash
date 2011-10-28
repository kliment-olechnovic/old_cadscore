#!/bin/bash

TEMP_FILE="/tmp/$(basename $0).$RANDOM.txt"

TMscore $1 $2 > $TEMP_FILE

echo TM_score `cat $TEMP_FILE | egrep "TM-score\s*=.*d0" | sed 's/TM-score\s*=\s*\(.*\)\s*(.*/\1/g'`
echo GDT_TS `cat $TEMP_FILE | egrep GDT-TS-score | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g'`
echo GDT_HA `cat $TEMP_FILE | egrep GDT-HA-score | sed 's/GDT-HA-score\s*=\s*\(.*\)\s*%(d<0\.5).*/\1/g'`
