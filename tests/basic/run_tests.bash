#!/bin/bash

cd $(dirname "$0")

CADSCORE_BIN_DIR=$1
INPUT_DIR=./input/
OUTPUT_DIR=./output/

rm -r -f $OUTPUT_DIR
mkdir $OUTPUT_DIR


exec &> $OUTPUT_DIR/output_log


for MODEL in $INPUT_DIR/*
do
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -q > $OUTPUT_DIR/quadruples_$(basename $MODEL) &
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -j > $OUTPUT_DIR/hyperfaces_$(basename $MODEL) &
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -s > $OUTPUT_DIR/contacts_summary_$(basename $MODEL) &
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -t > $OUTPUT_DIR/hyperfaces_summary_$(basename $MODEL) &
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -w > $OUTPUT_DIR/check_for_any_inter_chain_contact_$(basename $MODEL) &
  $CADSCORE_BIN_DIR/Voroprot2_calc.bash -f $MODEL -z 1.25 > $OUTPUT_DIR/check_for_inter_atom_clashes_$(basename $MODEL) &
  $CADSCORE_BIN_DIR/Voroprot2_print_interfaces_as_PyMol_graphics.bash -i $MODEL -f residue_id -s residue_id -g "(A50-A60)(B70-B80)" > $OUTPUT_DIR/graphics_$(basename $MODEL) &
done


(
DBNAME="db_all_contacts"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -a
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -a > $OUTPUT_DIR/$DBNAME/local_scores_absolute_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores
) &


(
DBNAME="db_inter_chain_contacts"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c -a
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -a > $OUTPUT_DIR/$DBNAME/local_scores_absolute_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores
) &


(
DBNAME="db_inter_chain_contacts_optimised"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c -q
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -a > $OUTPUT_DIR/$DBNAME/local_scores_absolute_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores
) &


(
DBNAME="db_interface_zone_contacts"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -z
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -a > $OUTPUT_DIR/$DBNAME/local_scores_absolute_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores
) &


(
DBNAME="db_custom_contacts"
for MODEL in $INPUT_DIR/model*
do
  $CADSCORE_BIN_DIR/CADscore_calc.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -i "(A40-A60)(B)"
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -w 1 > $OUTPUT_DIR/$DBNAME/local_scores_$(basename $MODEL)
  $CADSCORE_BIN_DIR/CADscore_read_local_scores.bash -D $OUTPUT_DIR/$DBNAME -t $INPUT_DIR/target -m $MODEL -c AA -a > $OUTPUT_DIR/$DBNAME/local_scores_absolute_$(basename $MODEL)
done
$CADSCORE_BIN_DIR/CADscore_read_global_scores.bash -D $OUTPUT_DIR/$DBNAME | sort -r | column -t > $OUTPUT_DIR/$DBNAME/global_scores
) &


(
DBNAME="db_scores_matrices"
$CADSCORE_BIN_DIR/CADscore_create_scores_matrices.bash -I $INPUT_DIR/ -O $OUTPUT_DIR/$DBNAME
) &


wait
