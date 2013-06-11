#!/bin/bash

CADSCORE_BIN_DIR=$1
OUTPUT_DIR=$2
TARGET_FILE=$3
MODEL_FILE=$4

$CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/db -t $TARGET_FILE -m $MODEL_FILE -a -n -u -q -e "rnadi.bash" -y -g -x
