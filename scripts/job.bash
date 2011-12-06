#!/bin/bash

JOB_DIRECTORY=$1

./score_many_many.bash $JOB_DIRECTORY/output $JOB_DIRECTORY/target "*" $JOB_DIRECTORY/model "*" $JOB_DIRECTORY/filter "*"

./assemble_scores_table.bash $JOB_DIRECTORY/output "*summary_table" "sm0_SS" > $JOB_DIRECTORY/output/scores
