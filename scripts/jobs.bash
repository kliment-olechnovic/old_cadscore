#!/bin/bash

JOBS_DIRECTORY=$1

for JOB_DIRECTORY in `find $JOBS_DIRECTORY ./refinement -mindepth 1 -maxdepth 1 -type d`
do
  ./job.bash $JOB_DIRECTORY
done
