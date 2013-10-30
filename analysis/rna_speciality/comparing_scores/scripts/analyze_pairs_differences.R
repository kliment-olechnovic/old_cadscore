rt=read.table("./output/decoys/pairs_differences_table.bak", header=TRUE, stringsAsFactors=FALSE);

rt=rt[which(abs(rt$diffs_AA)>0),];
rt=rt[which(abs(rt$diffs_AS)>0),];
rt=rt[which(abs(rt$diffs_SS)>0),];
rt=rt[which(abs(rt$diffs_MM)>0),];
rt=rt[which(abs(rt$diffs_rna_inf_norv)>0),];
rt=rt[which(abs(rt$diffs_rna_rmsd)>0),];
rt=rt[which(abs(rt$diffs_rna_di)>0),];
rt=rt[which(abs(rt$diffs_GDT_C3)>0),];
rt=rt[which(abs(rt$diffs_MP_clashscore)>0),];
rt=rt[which(abs(rt$diffs_MP_pct_badangles)>0),];
rt=rt[which(rt$maxs_rna_rmsd<5),];

mp_signs_diffs=sign(rt$diffs_MP_clashscore)-sign(rt$diffs_MP_pct_badangles);
rt=rt[which(mp_signs_diffs==0),];

#########################################

cadscore_variants=c("AA", "AS", "SS", "MM");

for(cadscore_variant in cadscore_variants)
{

used_thresholds=c();
set_sizes=c();
second_score_names=c();
subset_names=c();
subset_sizes=c();

thresholds=seq(0.1, 0.90, 0.01);

for(threshold in thresholds)
{
	t=rt;
	t=t[which(t$mins_rna_inf_norv>threshold),];
	
	N=length(t[[1]]);
	ids=1:N;
	
	CAD_score=sign(t[,paste("diffs_", cadscore_variant, sep="")])*ids;
	MP_score=sign(t$diffs_MP_clashscore)*ids;
	
	ev=data.frame(RNA_INF=sign(t$diffs_rna_inf_norv)*ids, RMSD=sign(t$diffs_rna_rmsd)*ids, RNA_DI=sign(t$diffs_rna_di)*ids, GDT_C3=sign(t$diffs_GDT_C3)*ids);
	
	for(ev_name in colnames(ev))
	{
		used_thresholds=c(used_thresholds, threshold);
		set_sizes=c(set_sizes, N);
		second_score_names=c(second_score_names, ev_name);
		subset_names=c(subset_names, paste("1. MP and CAD-score and ", ev_name, sep=""));
		subset_sizes=c(subset_sizes, length(intersect(MP_score, intersect(CAD_score, ev[, ev_name]))));
		
		used_thresholds=c(used_thresholds, threshold);
		set_sizes=c(set_sizes, N);
		second_score_names=c(second_score_names, ev_name);
		subset_names=c(subset_names, paste("2. MP and CAD-score without ", ev_name, sep=""));
		subset_sizes=c(subset_sizes, length(intersect(MP_score, setdiff(CAD_score, ev[, ev_name]))));
		
		used_thresholds=c(used_thresholds, threshold);
		set_sizes=c(set_sizes, N);
		second_score_names=c(second_score_names, ev_name);
		subset_names=c(subset_names, paste("3. MP and ", ev_name, " without CAD-score", sep=""));
		subset_sizes=c(subset_sizes, length(intersect(MP_score, setdiff(ev[, ev_name], CAD_score))));
		
		used_thresholds=c(used_thresholds, threshold);
		set_sizes=c(set_sizes, N);
		second_score_names=c(second_score_names, ev_name);
		subset_names=c(subset_names, paste("4. MP without CAD-score without ", ev_name, sep=""));
		subset_sizes=c(subset_sizes, length(setdiff(MP_score, union(CAD_score, ev[, ev_name]))));
	}
}

fractions=subset_sizes/set_sizes;

result=data.frame(min_RNA_INF=used_thresholds, set_size=set_sizes, second_score_name=second_score_names, subset_name=subset_names, subset_size=subset_sizes, fraction=fractions);
write.table(result, paste("./output/decoys/molprobity_results_", cadscore_variant, ".txt", sep=""), quote=TRUE, row.names=FALSE);

}
