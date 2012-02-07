#!/bin/bash

QTFIER="./qtfier_executables/QTFier"

CPUTYPE=$(uname -m)
if [ "$CPUTYPE" == "i686" ]
then
  QTFIER=$QTFIER"32"
else
  QTFIER=$QTFIER"64"
fi

INPUTFILE=$1
OUTPUTDIR=$(dirname $INPUTFILE)
OUTPUTDIR="$OUTPUTDIR/"
OUTPUTFILEBASE=$(basename $INPUTFILE .pdb)
OUTPUTFILE="$OUTPUTDIR/$OUTPUTFILEBASE.a.qtf"
OUTPUTLOG="$OUTPUTFILE.log"

(echo "input $INPUTFILE" | sed 's/\.*\S*\/\(\S*\).pdb/\1/' >&2 ; time -p $QTFIER $INPUTFILE $OUTPUTDIR) > /dev/null 2> $OUTPUTLOG
cat $OUTPUTFILE | egrep '^MODEL' | awk '{print "atoms",$3}' >> $OUTPUTLOG
cat $OUTPUTFILE | egrep '^MODEL' | awk '{print "vertices",$4}' >> $OUTPUTLOG
for i in 1 2 ; do cat $OUTPUTLOG | cut --delimiter " " --fields $i | paste -s ; done > $OUTPUTLOG.table
