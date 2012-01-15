#!/bin/bash

DATADIR="./data/"
SUMMARYDIR="./summary/"
TEMPFILE="$SUMMARYDIR/tempfile"

mkdir -p $SUMMARYDIR

./multirun.bash "./run_voroprot2.bash -p 0 -s 1 -f"
cat $DATADIR/*.vq.p0.s1.out.log.table > $TEMPFILE
(cat $TEMPFILE | head -1 ; cat $TEMPFILE | grep -v atoms) > $SUMMARYDIR/vq.p0.s1.table
rm $TEMPFILE

./multirun.bash "./run_qtfier.bash"
cat $DATADIR/*.qtf.log.table > $TEMPFILE
(cat $TEMPFILE | head -1 ; cat $TEMPFILE | grep -v atoms) > $SUMMARYDIR/qtf.table
rm $TEMPFILE

./multirun.bash "./run_voroprot2.bash -p 0 -s 0 -f"
cat $DATADIR/*.vq.p0.s0.out.log.table > $TEMPFILE
(cat $TEMPFILE | head -1 ; cat $TEMPFILE | grep -v atoms) > $SUMMARYDIR/vq.p0.s0.table
rm $TEMPFILE

./multirun.bash "./run_voroprot2.bash -p 1 -s 0 -f"
cat $DATADIR/*.vq.p1.s0.out.log.table > $TEMPFILE
(cat $TEMPFILE | head -1 ; cat $TEMPFILE | grep -v atoms) > $SUMMARYDIR/vq.p1.s0.table
rm $TEMPFILE
