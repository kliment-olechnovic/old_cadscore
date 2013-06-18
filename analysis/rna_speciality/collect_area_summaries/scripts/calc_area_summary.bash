#!/bin/bash

CADSCORE_BIN_DIR=$1
OUTPUT_DIR=$2
MODEL_FILE=$3

MODEL_BASENAME=$(basename $MODEL_FILE .ent.gz)

$CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL_FILE -s -u | egrep '^all_|sas' | sed "s/^/$MODEL_BASENAME /" | sed "s/^pdb//" > $OUTPUT_DIR/$MODEL_BASENAME.areas
