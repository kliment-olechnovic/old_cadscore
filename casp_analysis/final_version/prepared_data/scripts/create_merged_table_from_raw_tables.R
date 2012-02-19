casp9_scorestable=read.table("../input_data/casp9_CAD_scores_table", header=TRUE, stringsAsFactors=FALSE);
casp9_lga_sda_values=read.table("../input_data/casp9_LGA_sda_values", header=TRUE, stringsAsFactors=FALSE);
merged_table=merge(casp9_scorestable, casp9_lga_sda_values);
write.table(merged_table, "merged_table", quote=FALSE, row.names=FALSE);
