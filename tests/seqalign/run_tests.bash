#!/bin/bash

cd $(dirname "$0")

CADSCORE_BIN_DIR=$1
INPUT_DIR=./input/
OUTPUT_DIR=./output/

rm -r -f $OUTPUT_DIR
mkdir $OUTPUT_DIR


cat \
  "$INPUT_DIR/domain_1.alignment" \
  "$INPUT_DIR/domain_2.alignment" \
| ./compare_inter_chain_interfaces_contacts.bash \
  -v "$CADSCORE_BIN_DIR/voroprot2" \
  -1 "$INPUT_DIR/d1xq5a__1-d1xq5c__1.interchain.contacts" \
  -2 "$INPUT_DIR/d3bcqa__1-d3bcqc__1.interchain.contacts" \
  -a \
  -i \
> "$OUTPUT_DIR/output.out.txt" \
2> "$OUTPUT_DIR/output.log.txt" \

cat \
  "$OUTPUT_DIR/output.log.txt" \
  "$OUTPUT_DIR/output.out.txt" \
> "$OUTPUT_DIR/output.all.txt"

