#!/bin/bash

JOBS_DIRECTORY=$1

for JOB_DIRECTORY in `find $JOBS_DIRECTORY -type d -maxdepth 1`
do
  job.bash $JOB_DIRECTORY
done
