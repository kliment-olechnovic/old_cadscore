#!/bin/bash

cd $(dirname "$0")

CADSCORE_BIN_DIR=$1
INPUT_DIR=./input/
OUTPUT_DIR=./output/

rm -r -f $OUTPUT_DIR
mkdir $OUTPUT_DIR


for MODEL in $INPUT_DIR/*
do
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -q > $OUTPUT_DIR/quadruples_$(basename $MODEL)
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -j > $OUTPUT_DIR/hyperfaces_$(basename $MODEL)
done


DBNAME="db_all_contacts"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -a -n
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c SS -w 0 > $OUTPUT_DIR/$DBNAME/local_scores_SS_all_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c na_stacking -w 0 > $OUTPUT_DIR/$DBNAME/local_scores_SS_stacking_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c na_stacking_down -w 0 > $OUTPUT_DIR/$DBNAME/local_scores_SS_stacking_down_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c na_stacking_up -w 0 > $OUTPUT_DIR/$DBNAME/local_scores_SS_stacking_up_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c na_siding -w 0 > $OUTPUT_DIR/$DBNAME/local_scores_SS_siding_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores
