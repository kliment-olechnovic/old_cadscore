#!/bin/bash

R --vanilla --args ../collecting_scores/output/rp/merged_scores ./output/rp < ./scripts/analyze_correlations.R
cat ./output/rp/cor_table.txt | column -t > ./output/rp/cor_table_f.txt
mv ./output/rp/cor_table_f.txt ./output/rp/cor_table.txt
