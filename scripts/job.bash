#!/bin/bash

JOB_DIRECTORY=$1

./score_many_many.bash ./cad_score.bash $JOB_DIRECTORY/targets "*" $JOB_DIRECTORY/models "*" $JOB_DIRECTORY/output

./assemble_scores_table.bash $JOB_DIRECTORY/output "*summary_table" > $JOB_DIRECTORY/output/scores

R --silent --vanilla --args $JOB_DIRECTORY/output/scores $JOB_DIRECTORY/scores < tidy_cad_scores_table.R
