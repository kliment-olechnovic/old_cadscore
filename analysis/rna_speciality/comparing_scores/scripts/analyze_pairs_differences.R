args=commandArgs(trailingOnly=TRUE);
working_set=args[1];

rt=read.table(paste("./output/", working_set, "/pairs_differences_table.bak", sep=""), header=TRUE, stringsAsFactors=FALSE);

rt=rt[which(abs(rt$diffs_AA)>0),];
rt=rt[which(abs(rt$diffs_AS)>0),];
rt=rt[which(abs(rt$diffs_SS)>0),];
rt=rt[which(abs(rt$diffs_MM)>0),];
rt=rt[which(abs(rt$diffs_rna_inf_norv)>0),];
rt=rt[which(abs(rt$diffs_rna_rmsd)>0),];
rt=rt[which(abs(rt$diffs_rna_di)>0),];

rt$mp_sign_descriptor=sign(rt$diffs_MP_clashscore)*100+sign(rt$diffs_MP_pct_badangles)*10+sign(rt$diffs_MP_pct_badbonds);
allowed_descriptor_abs_values=c(1, 10, 100, 11, 101, 110, 111);
rt=rt[which(is.element(abs(rt$mp_sign_descriptor), allowed_descriptor_abs_values)),];
rt$mp_sign_descriptor=sign(rt$mp_sign_descriptor);

#########################################

cadscore_variants=c("AA", "AS", "SS", "MM");

for(cadscore_variant in cadscore_variants)
{

used_rmsd_thresholds=c();
used_rna_inf_thresholds=c();
set_sizes=c();
second_score_names=c();
subset_names=c();
subset_sizes=c();

rmsd_thresholds=1000;

rna_inf_thresholds=seq(0.4, 1.0, 0.05);

for(rmsd_threshold in rmsd_thresholds)
{

for(rna_inf_threshold in rna_inf_thresholds)
{
	t=rt;
	t=t[which(t$mins_rna_inf_norv>rna_inf_threshold),];
	t=t[which(t$maxs_rna_rmsd<rmsd_threshold),];
	
	N=length(t[[1]]);
	ids=1:N;
	
	CAD_score=sign(t[,paste("diffs_", cadscore_variant, sep="")])*ids;
	MP_score=sign(t$mp_sign_descriptor)*ids;
	
	ev=data.frame(RNA_INF=sign(t$diffs_rna_inf_norv)*ids, RMSD=sign(t$diffs_rna_rmsd)*ids, RNA_DI=sign(t$diffs_rna_di)*ids);
	
	for(ev_name in colnames(ev))
	{
		used_rmsd_thresholds=c(used_rmsd_thresholds, rmsd_threshold);
		used_rna_inf_thresholds=c(used_rna_inf_thresholds, rna_inf_threshold);
		set_sizes=c(set_sizes, N);
		second_score_names=c(second_score_names, ev_name);
		subset_names=c(subset_names, paste("1. MP and CAD-score and ", ev_name, sep=""));
		subset_sizes=c(subset_sizes, length(intersect(MP_score, intersect(CAD_score, ev[, ev_name]))));
		
		used_rmsd_thresholds=c(used_rmsd_thresholds, rmsd_threshold);
		used_rna_inf_thresholds=c(used_rna_inf_thresholds, rna_inf_threshold);
		set_sizes=c(set_sizes, N);
		second_score_names=c(second_score_names, ev_name);
		subset_names=c(subset_names, paste("2. MP and CAD-score without ", ev_name, sep=""));
		subset_sizes=c(subset_sizes, length(intersect(MP_score, setdiff(CAD_score, ev[, ev_name]))));
		
		used_rmsd_thresholds=c(used_rmsd_thresholds, rmsd_threshold);
		used_rna_inf_thresholds=c(used_rna_inf_thresholds, rna_inf_threshold);
		set_sizes=c(set_sizes, N);
		second_score_names=c(second_score_names, ev_name);
		subset_names=c(subset_names, paste("3. MP and ", ev_name, " without CAD-score", sep=""));
		subset_sizes=c(subset_sizes, length(intersect(MP_score, setdiff(ev[, ev_name], CAD_score))));
		
		used_rmsd_thresholds=c(used_rmsd_thresholds, rmsd_threshold);
		used_rna_inf_thresholds=c(used_rna_inf_thresholds, rna_inf_threshold);
		set_sizes=c(set_sizes, N);
		second_score_names=c(second_score_names, ev_name);
		subset_names=c(subset_names, paste("4. MP without CAD-score without ", ev_name, sep=""));
		subset_sizes=c(subset_sizes, length(setdiff(MP_score, union(CAD_score, ev[, ev_name]))));
	}
}

}

fractions=subset_sizes/set_sizes;

result=data.frame(max_RMSD=used_rmsd_thresholds, min_RNA_INF=used_rna_inf_thresholds, set_size=set_sizes, second_score_name=second_score_names, subset_name=subset_names, subset_size=subset_sizes, fraction=fractions);
write.table(result, paste("./output/", working_set, "/molprobity_results_", cadscore_variant, ".txt", sep=""), quote=TRUE, row.names=FALSE);

}
