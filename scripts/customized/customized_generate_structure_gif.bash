#!/bin/bash

VOROPROT=$1
INPUT=$2
WIDTH=$3
HEIGHT=$4
OUTPUT=$5

if [ -z "$VOROPROT" ] || [ -z "$INPUT" ] || [ -z "$WIDTH" ] || [ -z "$HEIGHT" ] || [ -z "$OUTPUT" ]
then
  echo "Fatal error: parameters not provided" 1>&2
  exit 1
fi

TMP_DIR=$(mktemp -d)

SCRIPT_FILE="$TMP_DIR/script.py"

cat << EOF > $SCRIPT_FILE
from pymol.cgo import *
from pymol import cmd

EOF

cat $INPUT | $VOROPROT --mode collect-atoms | $VOROPROT --mode print-optimal-rotation | sed 's/.*/cmd.do("&")/' >> $SCRIPT_FILE

cat << EOF >> $SCRIPT_FILE

cmd.do("util.cbc")
cmd.do("hide all")
cmd.do("show cartoon")
cmd.do("center")

cmd.do("rotate y, -4")
cmd.do("save $TMP_DIR/i1.png")

cmd.do("rotate y, 2")
cmd.do("save $TMP_DIR/i2.png")

cmd.do("rotate y, 2")
cmd.do("save $TMP_DIR/i3.png")

cmd.do("rotate y, 2")
cmd.do("save $TMP_DIR/i4.png")

cmd.do("rotate y, 2")
cmd.do("save $TMP_DIR/i5.png")

EOF

pymol $INPUT $SCRIPT_FILE -c -W $WIDTH -H $HEIGHT

convert -delay 20 -loop 0 $TMP_DIR/i1.png $TMP_DIR/i2.png $TMP_DIR/i3.png $TMP_DIR/i4.png $TMP_DIR/i5.png $TMP_DIR/i4.png $TMP_DIR/i3.png $TMP_DIR/i2.png $OUTPUT.gif

rm -r "$TMP_DIR"
