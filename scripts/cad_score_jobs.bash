#!/bin/bash

JOBS_DIRECTORY=$1

if [ ! -d "$JOBS_DIRECTORY" ]
then
  echo "Jobs directory \"$JOBS_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

for JOB_DIRECTORY in `find $JOBS_DIRECTORY -mindepth 1 -maxdepth 1 -type d`
do
  ./cad_score_job.bash $JOB_DIRECTORY
done
