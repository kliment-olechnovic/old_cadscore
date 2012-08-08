t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$domain>0),];

t_dssp_multimap=read.table("targets_dssp_info", header=TRUE, stringsAsFactors=FALSE);

output_directory="dssp_dependent_correlation_plots";
dir.create(output_directory);

t_dssp_ids=t_dssp_multimap[which(t_dssp_multimap$key=="target"), 2];
t_dssp_all_residues_count=t_dssp_multimap[which(t_dssp_multimap$key=="all_residues_count"), 2];
t_dssp_helix_residues_count=t_dssp_multimap[which(t_dssp_multimap$key=="helix_residues_count"), 2];
t_dssp_strand_residues_count=t_dssp_multimap[which(t_dssp_multimap$key=="strand_residues_count"), 2];
t_dssp_helicity=t_dssp_helix_residues_count/t_dssp_all_residues_count;
t_dssp_strandity=t_dssp_strand_residues_count/t_dssp_all_residues_count;

png(paste(output_directory, "/targets_helicity.png", sep=""), height=5, width=5, units="in", res=300);
hist(t_dssp_helicity, breaks=30, xlab="Targets helicity", main="Histogram of targets helicity");
dev.off();

png(paste(output_directory, "/targets_strandity.png", sep=""), height=5, width=5, units="in", res=300);
hist(t_dssp_strandity, breaks=30, xlab="Targets strandity", main="Histogram of targets strandity");
dev.off();

probs_list=c(0.1, 0.9);
helicity_threshold=as.numeric(quantile(t_dssp_helicity, probs=probs_list)[2][1]);
strandity_threshold=as.numeric(quantile(t_dssp_strandity, probs=probs_list)[2][1]);
set_helix=t_dssp_ids[which(t_dssp_helicity>helicity_threshold)];
set_strand=t_dssp_ids[which(t_dssp_strandity>strandity_threshold)];

targets_ids=t$target*10+t$domain;
helix_t=t[which(is.element(targets_ids, set_helix)),];
strand_t=t[which(is.element(targets_ids, set_strand)),];

png(paste(output_directory, "/dssp_dependent_correlation_GDT_TS_vs_MM.png", sep=""), height=4.8, width=4.4, units="in", res=600);
plot(x=c(0,1), y=c(0,1), type="n", col="black", xlab="", ylab="", main="")
points(y=helix_t$MM, x=helix_t$CASP_GDT_TS, col="darkorange2", pch=0, cex=0.7);
points(y=strand_t$MM, x=strand_t$CASP_GDT_TS, col="cyan4", pch=1, cex=0.7);
dev.off();

cor_names=c("all", "helix", "strand");
counts=c(length(t_dssp_ids), length(set_helix), length(set_strand));
cor_values_pearson=c(cor(t$MM, t$CASP_GDT_TS, method="pearson"), cor(helix_t$MM, helix_t$CASP_GDT_TS, method="pearson"), cor(strand_t$MM, strand_t$CASP_GDT_TS, method="pearson"));
cor_values_spearman=c(cor(t$MM, t$CASP_GDT_TS, method="spearman"), cor(helix_t$MM, helix_t$CASP_GDT_TS, method="spearman"), cor(strand_t$MM, strand_t$CASP_GDT_TS, method="spearman"));
cor_table=data.frame(type=cor_names, count=counts, cor_pearson=cor_values_pearson, cor_spearman=cor_values_spearman);

write.table(cor_table, "dssp_dependent_correlation_GDT_TS_vs_MM_table", quote=FALSE, row.names=FALSE);

thresholds_table=data.frame(threshold_name=c("helices", "strands"), value=c(helicity_threshold, strandity_threshold));
write.table(thresholds_table, "used_thresholds", quote=FALSE, row.names=FALSE);
