#!/bin/bash

JOB_DIRECTORY=$1

if [ ! -d "$JOB_DIRECTORY" ]
then
  echo "Job directory \"$JOB_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

./cad_score_many_many.bash -O $JOB_DIRECTORY/output -T $JOB_DIRECTORY/target -M $JOB_DIRECTORY/model -F $JOB_DIRECTORY/filter

./assemble_scores_table.bash -S $JOB_DIRECTORY/output -p "*summary_table" -f "sm0_SS" > $JOB_DIRECTORY/output/scores
