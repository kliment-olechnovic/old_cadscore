#!/bin/bash

INPUT_DIR="models"
OUTPUT_DIR="shrinked_models"

for L in `cat representatives_table | tail -n +2 | tr " " ","`
do
  T=$(echo $L | cut --delimiter ',' --fields 1)
  D=$(echo $L | cut --delimiter ',' --fields 2)
  G=$(echo $L | cut --delimiter ',' --fields 3)
  TN="T0"$T"-D"$D
  MN="T0"$T"TS"$G"_1-D"$D".lga"
  mkdir -p "$OUTPUT_DIR/$TN/target"
  mkdir -p "$OUTPUT_DIR/$TN/model"
  cp "$INPUT_DIR/$TN/target/$TN" "$OUTPUT_DIR/$TN/target/$TN"
  for FACTOR_INT in {90..110..1}
  do
    FACTOR_FLOAT=$(echo "$FACTOR_INT/100" | bc -l | awk '{printf "%f\n", $0}')
    ./shrink_pdb.pl "$INPUT_DIR/$TN/model/$MN" "$OUTPUT_DIR/$TN/model/$MN""___$FACTOR_INT" $FACTOR_FLOAT
  done
done
