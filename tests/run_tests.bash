#!/bin/bash

cp ../Release/voroprot2 ../scripts/voroprot2
CADSCORE_BIN_DIR=../scripts/
INPUT_DIR=./input/
OUTPUT_DIR=./output/


rm -r ./output
mkdir ./output


for MODEL in $INPUT_DIR/*
do
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -q > $OUTPUT_DIR/quadruples_$(basename $MODEL)
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -j > $OUTPUT_DIR/hyperfaces_$(basename $MODEL)
  $CADSCORE_BIN_DIR/Voroprot2_print_interfaces_as_PyMol_graphics.bash -i $MODEL -f residue_id -s residue_id -g "(A50-A60)(B70-B80)" > $OUTPUT_DIR/graphics_$(basename $MODEL)
done


DBNAME="db_all_contacts"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -a
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores


DBNAME="db_inter_chain_contacts"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c -a
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores


DBNAME="db_inter_chain_contacts_optimised"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c -q
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores


DBNAME="db_custom_contacts"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -i "(A40-A60)(B)"
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores


DBNAME="db_scores_matrices"
$CADSCORE_BIN_DIR/CADscore_create_scores_matrices.bash -I $INPUT_DIR/ -O $OUTPUT_DIR/$DBNAME
