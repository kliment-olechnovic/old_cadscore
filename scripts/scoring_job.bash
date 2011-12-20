#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

###########################################

JOB_DIRECTORY=$1

###########################################

if [ ! -d "$JOB_DIRECTORY" ]
then
  echo "Job directory \"$JOB_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

$SCRIPT_DIRECTORY/scoring_looper.bash -O $JOB_DIRECTORY/db -T $JOB_DIRECTORY/target -M $JOB_DIRECTORY/model

$SCRIPT_DIRECTORY/util_cat_tables.bash -S $JOB_DIRECTORY/db -p "*scoring_report/*/table_row" -f "AA" > $JOB_DIRECTORY/db/all_scores_table
