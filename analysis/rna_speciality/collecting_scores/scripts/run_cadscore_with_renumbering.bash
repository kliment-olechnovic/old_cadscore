#!/bin/bash

CADSCORE_BIN_DIR=$1
OUTPUT_DIR=$2
TARGET_FILE=$3
MODEL_FILE=$4

TMP_DIR=$(mktemp -d)

MODEL_BASENAME=$(basename $MODEL_FILE)
RENUMBERED_MODEL_FILE="$TMP_DIR/$MODEL_BASENAME"
(cat $TARGET_FILE | $CADSCORE_BIN_DIR/voroprot2 --mode collect-atoms ; cat $MODEL_FILE | $CADSCORE_BIN_DIR/voroprot2 --mode collect-atoms) | $CADSCORE_BIN_DIR/voroprot2 --mode x-renumber-residues-by-reference --match 10 --mismatch -20 --gap-start -50 --gap-extension 0 --output-in-pdb-format > $RENUMBERED_MODEL_FILE

$CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/db -t $TARGET_FILE -m $RENUMBERED_MODEL_FILE -a -n -u -q -e "rnadi.bash" -y -g -x

rm -r $TMP_DIR
