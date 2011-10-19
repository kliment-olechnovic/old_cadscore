#!/bin/bash

INPUT_FILE=$1
INPUT_FILE_BASE=$(basename $INPUT_FILE)
OUTPUT_PREFIX=$2
CONTACTS_OUTPUT_FILE=$OUTPUT_PREFIX"/contacts_"$INPUT_FILE_BASE
LOG_OUTPUT_FILE=$OUTPUT_PREFIX"/log_"$INPUT_FILE_BASE

./voroprot2 \
--mode ciac \
--radius-classes ./resources/vdwr_classes.txt \
--radius-members ./resources/vdwr_members.txt \
--depth 3 \
--probe 1.4 \
< $INPUT_FILE \
1> $CONTACTS_OUTPUT_FILE \
2> $LOG_OUTPUT_FILE
