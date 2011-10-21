#!/bin/bash

INPUT_FILE=$1
INPUT_FILE_BASE=$(basename $INPUT_FILE)
CONTACTS_OUTPUT_PREFIX=$2
CONTACTS_OUTPUT_FILE=$CONTACTS_OUTPUT_PREFIX"/contacts_"$INPUT_FILE_BASE
LOG_OUTPUT_PREFIX=$3
LOG_OUTPUT_FILE=$LOG_OUTPUT_PREFIX"/log_"$INPUT_FILE_BASE

timeout 10s \
cat $INPUT_FILE \
| \
./voroprot \
--mode collect-atoms \
--radius-classes ./resources/vdwr_classes.txt \
--radius-members ./resources/vdwr_members.txt \
| \
./voroprot \
--mode construct-inter-atom-contacts \
--depth 3 \
--probe 1.4 \
1> $CONTACTS_OUTPUT_FILE \
2> $LOG_OUTPUT_FILE

RETURN_VALUE=$?
[ $RETURN_VALUE -ne 0 ] && echo "Failed to process $INPUT_FILE"
