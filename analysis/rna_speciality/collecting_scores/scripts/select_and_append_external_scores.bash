#!/bin/bash

INPUT_FILE="./input/external_scores"
SELECTION_FILE="./output/selected_external_scores"
MERGED_SCORES_FILE="./output/merged_scores"

echo "target model ext_rmsd ext_di_all ext_inf_all ext_inf_wc ext_inf_nwc ext_inf_stacking ext_clash_score" > $SELECTION_FILE.uf
cat $INPUT_FILE | tail -n +2 | awk '{ print $1 "_solution_0 " $1 "_" $2 "_" $3 " " $4 " " $6 " " $8 " " $10 " " $12 " " $14 " " $16 }' | sort -r >> $SELECTION_FILE.uf
cat $SELECTION_FILE.uf | column -t > $SELECTION_FILE
rm $SELECTION_FILE.uf

R --vanilla << EOF
ct=read.table("./output/calculated_scores", header=TRUE, stringsAsFactors=FALSE);
et=read.table("./output/selected_external_scores", header=TRUE, stringsAsFactors=FALSE);
mt=merge(ct, et);
write.table(mt, "./output/merged_scores.uf", quote=FALSE, row.names=FALSE, col.names=TRUE);
EOF
cat $MERGED_SCORES_FILE.uf | column -t > $MERGED_SCORES_FILE
rm $MERGED_SCORES_FILE.uf
