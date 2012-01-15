#!/bin/bash

DATADIR="./data/"
SUMMARYDIR="./summary/"

mkdir -p $SUMMARYDIR

./multirun.bash "./run_voroprot2.bash -p 0 -s 1 -f"
cat $DATADIR/*.vq.p0.s1.out.log.table > $SUMMARYDIR/vq.p0.s1.table

./multirun.bash "./run_qtfier.bash"
cat $DATADIR/*.qtf.log.table > $SUMMARYDIR/qtf.table

./multirun.bash "./run_voroprot2.bash -p 0 -s 0 -f"
cat $DATADIR/*.vq.p0.s0.out.log.table > $SUMMARYDIR/vq.p0.s0.table

./multirun.bash "./run_voroprot2.bash -p 1 -s 0 -f"
cat $DATADIR/*.vq.p1.s0.out.log.table > $SUMMARYDIR/vq.p1.s0.table
