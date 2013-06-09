#!/bin/bash

R --vanilla --args ../collecting_scores/output/rp/merged_scores ./output/rp < ./scripts/analyze_correlations.R
cat ./output/rp/cor_table.txt | column -t > ./output/rp/cor_table_f.txt
mv ./output/rp/cor_table_f.txt ./output/rp/cor_table.txt

R --vanilla --args ../collecting_scores/output/decoys/calculated_scores ./output/decoys < ./scripts/analyze_correlations.R
cat ./output/decoys/cor_table.txt | column -t > ./output/decoys/cor_table_f.txt
mv ./output/decoys/cor_table_f.txt ./output/decoys/cor_table.txt
