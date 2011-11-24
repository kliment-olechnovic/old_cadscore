#!/bin/bash

JOB_DIRECTORY=$1

./score_many_many.bash ./cad_score.bash $JOB_DIRECTORY/target "*" $JOB_DIRECTORY/model "*" $JOB_DIRECTORY/output

./assemble_scores_table.bash $JOB_DIRECTORY/output "*summary_table" > $JOB_DIRECTORY/output/scores

