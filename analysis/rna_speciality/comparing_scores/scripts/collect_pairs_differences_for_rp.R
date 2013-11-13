t=read.table("../collecting_scores/output/rp/merged_scores", header=TRUE, stringsAsFactors=FALSE);

t=t[which(is.finite(t$rna_rmsd)),];
t=t[which(is.finite(t$rna_inf_norv)),];
t$rna_di=t$rna_rmsd/t$rna_inf_norv;
t=t[which(is.finite(t$rna_di)),];

targets=union(sort(t$target), sort(t$target));

max_vector_size=0;
for(target in targets)
{
	target_size=length(which(t$target==target));
	max_vector_size=max_vector_size+(target_size*(target_size-1)/2);
}

mins_AA=numeric(max_vector_size);
mins_AS=numeric(max_vector_size);
mins_SS=numeric(max_vector_size);
mins_MM=numeric(max_vector_size);
mins_rna_inf_norv=numeric(max_vector_size);
mins_rna_rmsd=numeric(max_vector_size);
maxs_rna_rmsd=numeric(max_vector_size);
maxs_rna_di=numeric(max_vector_size);
diffs_AA=numeric(max_vector_size);
diffs_AS=numeric(max_vector_size);
diffs_SS=numeric(max_vector_size);
diffs_MM=numeric(max_vector_size);
diffs_rna_inf_norv=numeric(max_vector_size);
diffs_rna_rmsd=numeric(max_vector_size);
diffs_rna_di=numeric(max_vector_size);
diffs_MP_clashscore=numeric(max_vector_size);
diffs_MP_pct_badangles=numeric(max_vector_size);
diffs_MP_pct_badbonds=numeric(max_vector_size);

k=1;
for(target in targets)
{
	st=t[which(t$target==target),];
	N=length(st[[1]]);
	for(i in 1:(N-1))
	{
		for(j in (i+1):N)
		{
			mins_AA[k]=min(st$AA[i], st$AA[j]);
			mins_AS[k]=min(st$AS[i], st$AS[j]);
			mins_SS[k]=min(st$SS[i], st$SS[j]);
			mins_MM[k]=min(st$MM[i], st$MM[j]);
			mins_rna_inf_norv[k]=min(st$rna_inf_norv[i], st$rna_inf_norv[j]);
			mins_rna_rmsd[k]=min(st$rna_rmsd[i], st$rna_rmsd[j]);
			maxs_rna_rmsd[k]=max(st$rna_rmsd[i], st$rna_rmsd[j]);
			maxs_rna_di[k]=max(st$rna_di[i], st$rna_di[j]);
			diffs_AA[k]=st$AA[i]-st$AA[j];
			diffs_AS[k]=st$AS[i]-st$AS[j];
			diffs_SS[k]=st$SS[i]-st$SS[j];
			diffs_MM[k]=st$MM[i]-st$MM[j];
			diffs_rna_inf_norv[k]=st$rna_inf_norv[i]-st$rna_inf_norv[j];
			diffs_rna_rmsd[k]=st$rna_rmsd[j]-st$rna_rmsd[i];
			diffs_rna_di[k]=st$rna_di[j]-st$rna_di[i];
			diffs_MP_clashscore[k]=st$MP_clashscore[j]-st$MP_clashscore[i];
			diffs_MP_pct_badangles[k]=st$MP_pct_badangles[j]-st$MP_pct_badangles[i];
			diffs_MP_pct_badbonds[k]=st$MP_pct_badbonds[j]-st$MP_pct_badbonds[i];
			k=k+1;
		}
	}
}

pdt=data.frame(
	mins_AA=mins_AA,
	mins_AS=mins_AS,
	mins_SS=mins_SS,
	mins_MM=mins_MM,
	mins_rna_inf_norv=mins_rna_inf_norv,
	mins_rna_rmsd=mins_rna_rmsd,
	maxs_rna_rmsd=maxs_rna_rmsd,
	maxs_rna_di=maxs_rna_di,
	diffs_AA=diffs_AA,
	diffs_AS=diffs_AS,
	diffs_SS=diffs_SS,
	diffs_MM=diffs_MM,
	diffs_rna_inf_norv=diffs_rna_inf_norv, 
	diffs_rna_rmsd=diffs_rna_rmsd, 
	diffs_rna_di=diffs_rna_di,
	diffs_MP_clashscore=diffs_MP_clashscore,
	diffs_MP_pct_badangles=diffs_MP_pct_badangles,
	diffs_MP_pct_badbonds=diffs_MP_pct_badbonds);

write.table(pdt, "./output/rp/pairs_differences_table.bak", quote=FALSE, row.names=FALSE);
