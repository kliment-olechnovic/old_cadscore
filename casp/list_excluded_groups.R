cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$server==1),];
t8=t[which(t$CASP==8),];
t9=t[which(t$CASP==9),];

png(paste(cmd_args[2], "casp8_ratio_hist.png", sep=""), height=10, width=10, units="in", res=200);
hist((t8$model_atoms_count/t8$model_residues_count), breaks=100, xlim=c(0, 10));
points(y=c(0, length(t8$model_atoms_count)), x=c(6.5, 6.5), col="red", type="l");
dev.off();

png(paste(cmd_args[2], "casp9_ratio_hist.png", sep=""), height=10, width=10, units="in", res=200);
hist((t9$model_atoms_count/t9$model_residues_count), breaks=100, xlim=c(0, 10));
points(y=c(0, length(t9$model_atoms_count)), x=c(6.5, 6.5), col="red", type="l");
dev.off();

ex8=t8$group[which(t8$solid_group==0)];
ex8=union(ex8, ex8);
casp_8_server_groups=read.table("casp_8_servers.txt", header=FALSE);
casp_8_excluded_server_groups=casp_8_server_groups[which(is.element(casp_8_server_groups[[1]], ex8)),];

ex9=t9$group[which(t9$solid_group==0)];
ex9=union(ex9, ex9);
casp_9_server_groups=read.table("casp_9_servers.txt", header=FALSE);
casp_9_excluded_server_groups=casp_9_server_groups[which(is.element(casp_9_server_groups[[1]], ex9)),];

write.table(casp_8_excluded_server_groups, paste(cmd_args[2], "casp_8_excluded_server_groups.txt", sep=""), quote=FALSE, row.names=FALSE, col.names=FALSE);
write.table(casp_9_excluded_server_groups, paste(cmd_args[2], "casp_9_excluded_server_groups.txt", sep=""), quote=FALSE, row.names=FALSE, col.names=FALSE);
