#!/bin/bash

MCANNOTATE="./MC-Annotate"

INPUT=$1

TMP_DIR=$(mktemp -d)

$MCANNOTATE $INPUT > $TMP_DIR/all

cat $TMP_DIR/all | sed -n -e '/Adjacent stackings/,/Non-Adjacent stackings/ p' > $TMP_DIR/as
cat $TMP_DIR/all | sed -n -e '/Non-Adjacent stackings/,/Number of stackings/ p' > $TMP_DIR/nas
cat $TMP_DIR/as $TMP_DIR/nas | egrep " : " | sed "s/^/$(basename $INPUT) /"

rm -r $TMP_DIR
