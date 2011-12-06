#!/bin/bash

JOBS_DIRECTORY=$1

for JOB_DIRECTORY in `find $JOBS_DIRECTORY -mindepth 1 -maxdepth 1 -type d`
do
  ./cad_score_job.bash $JOB_DIRECTORY
done
