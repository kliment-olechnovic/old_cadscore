#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)
CAD_SCORE_LOOPER=$SCRIPT_DIRECTORY/cad_score_looper.bash
CAD_SCORE_TABLE_ASSEMBLER=$SCRIPT_DIRECTORY/cad_score_table_assembler.bash

JOB_DIRECTORY=$1

if [ ! -d "$JOB_DIRECTORY" ]
then
  echo "Job directory \"$JOB_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

$CAD_SCORE_LOOPER -O $JOB_DIRECTORY/output -T $JOB_DIRECTORY/target -M $JOB_DIRECTORY/model -F $JOB_DIRECTORY/filter

$CAD_SCORE_TABLE_ASSEMBLER -S $JOB_DIRECTORY/output -p "*summary_table" -f "sm0_SS" > $JOB_DIRECTORY/output/scores
