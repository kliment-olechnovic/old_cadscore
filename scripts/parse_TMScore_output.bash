#!/bin/bash

echo TM_score `cat $1 | egrep "TM-score\s*=.*d0" | sed 's/TM-score\s*=\s*\(.*\)\s*(.*/\1/g'`
echo GDT_TS `cat $1 | egrep GDT-TS-score | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g'`
echo GDT_HA `cat $1 | egrep GDT-HA-score | sed 's/GDT-HA-score\s*=\s*\(.*\)\s*%(d<0\.5).*/\1/g'`
