#!/bin/bash

MCANNOTATE="./MC-Annotate"

INPUT=$1

TMP_DIR=$(mktemp -d)

ONLY_ATOMS_INPUT="$TMP_DIR/atoms.pdb"
cat $INPUT | awk '/^END/{exit}1' | egrep '^ATOM' > $ONLY_ATOMS_INPUT

$MCANNOTATE $ONLY_ATOMS_INPUT | sed -n -e '/Base-pairs/,/-------/ p' | egrep " : " | sed "s/^/$(basename $INPUT) /"

rm -r $TMP_DIR
