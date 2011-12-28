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

$SCRIPT_DIRECTORY/db_set_multiple_scoring_reports.bash -D $JOB_DIRECTORY/db -T $JOB_DIRECTORY/target -M $JOB_DIRECTORY/model

$SCRIPT_DIRECTORY/db_set_scoring_summary.bash -D $JOB_DIRECTORY/db
