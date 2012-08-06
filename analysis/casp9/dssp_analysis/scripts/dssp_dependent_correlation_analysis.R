t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$domain>0),];

t_dssp_multimap=read.table("targets_dssp_info", header=TRUE, stringsAsFactors=FALSE);

output_directory="dssp_dependent_correlation_plots";
dir.create(output_directory);

t_dssp_ids=t_dssp_multimap[which(t_dssp_multimap$key=="target"), 2];
t_dssp_all_residues_count=t_dssp_multimap[which(t_dssp_multimap$key=="all_residues_count"), 2];
t_dssp_helix_residues_count=t_dssp_multimap[which(t_dssp_multimap$key=="helix_residues_count"), 2];
t_dssp_strand_residues_count=t_dssp_multimap[which(t_dssp_multimap$key=="strand_residues_count"), 2];
t_dssp_helicity=(t_dssp_helix_residues_count-t_dssp_strand_residues_count)/t_dssp_all_residues_count;

png(paste(output_directory, "/targets_helicity.png", sep=""), height=5, width=5, units="in", res=300);
hist(t_dssp_helicity, breaks=30, xlab="Targets helicity", main="Histogram of targets helicity");
dev.off();

helicity_threshold=0.0;
set_helix=t_dssp_ids[which(t_dssp_helicity>helicity_threshold)];
set_strand=t_dssp_ids[which((0-t_dssp_helicity)>helicity_threshold)];

targets_ids=t$target*10+t$domain;
helix_t=t[which(is.element(targets_ids, set_helix)),];
strand_t=t[which(is.element(targets_ids, set_strand)),];

png(paste(output_directory, "/dssp_dependent_correlation_GDT_TS_vs_MM.png", sep=""), height=5, width=5, units="in", res=300);
plot(x=c(0,1), y=c(0,1), type="l", col="black", ylab="MM", xlab="GDT_TS", main="")
points(y=helix_t$MM, x=helix_t$CASP_GDT_TS, col="red", pch=15, cex=0.5);
points(y=strand_t$MM, x=strand_t$CASP_GDT_TS, col="green", pch=16, cex=0.5);
dev.off();

cor_names=c("all", "helix", "strand");
counts=c(length(t_dssp_ids), length(set_helix), length(set_strand));
cor_values_pearson=c(cor(t$MM, t$CASP_GDT_TS, method="pearson"), cor(helix_t$MM, helix_t$CASP_GDT_TS, method="pearson"), cor(strand_t$MM, strand_t$CASP_GDT_TS, method="pearson"));
cor_values_spearman=c(cor(t$MM, t$CASP_GDT_TS, method="spearman"), cor(helix_t$MM, helix_t$CASP_GDT_TS, method="spearman"), cor(strand_t$MM, strand_t$CASP_GDT_TS, method="spearman"));
cor_table=data.frame(type=cor_names, count=counts, cor_pearson=cor_values_pearson, cor_spearman=cor_values_spearman);

write.table(cor_table, "dssp_dependent_correlation_GDT_TS_vs_MM_table", quote=FALSE, row.names=FALSE);
