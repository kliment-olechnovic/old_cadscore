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
OUTPUTLOG="$INPUTFILE.qtf.log"

(echo "input $INPUTFILE" >&2 ; time -p $QTFIER $INPUTFILE $OUTPUTDIR) > /dev/null 2> $OUTPUTLOG

