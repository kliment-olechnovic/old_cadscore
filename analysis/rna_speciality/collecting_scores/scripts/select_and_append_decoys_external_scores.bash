#!/bin/bash

MERGED_SCORES_FILE="./output/decoys/merged_scores"

R --vanilla << EOF
ct=read.table("./output/decoys/calculated_scores", header=TRUE, stringsAsFactors=FALSE);
et=read.table("./input/decoys_external_scores", header=TRUE, stringsAsFactors=FALSE);
mpt=read.table("./input/decoys_molprobity_scores", header=TRUE, stringsAsFactors=FALSE);
mt=merge(merge(ct, et), mpt);
write.table(mt, "./output/decoys/merged_scores.uf", quote=FALSE, row.names=FALSE, col.names=TRUE);
EOF
cat $MERGED_SCORES_FILE.uf | column -t > $MERGED_SCORES_FILE
rm $MERGED_SCORES_FILE.uf
