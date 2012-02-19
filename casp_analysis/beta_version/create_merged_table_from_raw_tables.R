casp9_scorestable=read.table("casp9_scorestable", header=TRUE, stringsAsFactors=FALSE);
casp9_lga_sda_values=read.table("casp9_lga_sda_values", header=TRUE, stringsAsFactors=FALSE);
merged_table=merge(casp9_scorestable, casp9_lga_sda_values);
write.table(merged_table, "merged_table", quote=FALSE, row.names=FALSE);
