#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)
CAD_SCORE_LOOPER=$SCRIPT_DIRECTORY/cad_score_looper.bash
CAD_SCORE_TABLE_ASSEMBLER=$SCRIPT_DIRECTORY/cad_score_table_assembler.bash

###########################################

JOB_DIRECTORY=$1

###########################################

if [ ! -d "$JOB_DIRECTORY" ]
then
  echo "Job directory \"$JOB_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

$SCRIPT_DIRECTORY/cad_score_looper.bash -O $JOB_DIRECTORY/db -T $JOB_DIRECTORY/target -M $JOB_DIRECTORY/model

$SCRIPT_DIRECTORY/util_cat_tables.bash -S $JOB_DIRECTORY/db -p "*cad_score_table" -f "AA" > $JOB_DIRECTORY/db/cad_scores_table
