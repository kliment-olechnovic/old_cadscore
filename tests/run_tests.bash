#!/bin/bash

rm -r ./output
mkdir ./output

for MODEL in ./input/*
do
  ../scripts/Voroprot2_calc.bash -f $MODEL -q > ./output/quadruples_$(basename $MODEL)
  ../scripts/Voroprot2_calc.bash -f $MODEL -j > ./output/hyperfaces_$(basename $MODEL)
done


DBNAME="db_all_contacts"
for MODEL in ./input/model*
do
  ../scripts/CADscore_calc.bash -D ./output/$DBNAME -t ./input/target -m $MODEL
  ../scripts/CADscore_read_local_scores.bash -D ./output/$DBNAME -t ./input/target -m $MODEL -c AA -w 1 > ./output/$DBNAME/local_scores_$(basename $MODEL)
done
../scripts/CADscore_read_global_scores.bash -D ./output/$DBNAME | sort -r | column -t > ./output/$DBNAME/global_scores


DBNAME="db_inter_chain_contacts"
for MODEL in ./input/model*
do
  ../scripts/CADscore_calc.bash -D ./output/$DBNAME -t ./input/target -m $MODEL -c
  ../scripts/CADscore_read_local_scores.bash -D ./output/$DBNAME -t ./input/target -m $MODEL -c AA -w 1 > ./output/$DBNAME/local_scores_$(basename $MODEL)
done
../scripts/CADscore_read_global_scores.bash -D ./output/$DBNAME | sort -r | column -t > ./output/$DBNAME/global_scores
