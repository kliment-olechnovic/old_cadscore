args=commandArgs(trailingOnly=TRUE);
input_table_file=args[1];
input_ids_file=args[2];
output_table_file=args[3];

t=read.table(input_table_file, header=FALSE, stringsAsFactors=FALSE);

ids=read.table(input_ids_file, header=FALSE, stringsAsFactors=FALSE)[[1]];

st=t[which(is.element(t[[1]], ids)),];

write.table(st, output_table_file, quote=FALSE, row.names=FALSE, col.names=FALSE);
