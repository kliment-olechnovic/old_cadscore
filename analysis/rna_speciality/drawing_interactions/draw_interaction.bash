#!/bin/bash

WIDTH=150
HEIGHT=150

OUTPUTDIR=$1
INPUT=$2
A_CHAIN=$3
A_RESNUM=$4
B_CHAIN=$5
B_RESNUM=$6

INPUT_BASENAME=$(basename $INPUT .pdb)

mkdir -p $OUTPUTDIR
OUTPUT="$OUTPUTDIR/$INPUT_BASENAME-$A_CHAIN$A_RESNUM-$B_CHAIN$B_RESNUM.png"

TMP_DIR=$(mktemp -d)

SCRIPT_FILE="$TMP_DIR/script.py"

cat << EOF > $SCRIPT_FILE
from pymol.cgo import *
from pymol import cmd

cmd.hide()

cmd.select('selab', 'resi $A_RESNUM and chain $A_CHAIN or resi $B_RESNUM and chain $B_CHAIN');

cmd.color('red', 'resi $A_RESNUM and chain $A_CHAIN')
cmd.color('green', 'resi $B_RESNUM and chain $B_CHAIN')

cmd.show('sticks', 'selab')

cmd.orient('selab')
cmd.center('selab')
cmd.zoom('selab')

cmd.do('save $OUTPUT')

EOF

pymol $INPUT $SCRIPT_FILE -c -W $WIDTH -H $HEIGHT

rm -r "$TMP_DIR"
