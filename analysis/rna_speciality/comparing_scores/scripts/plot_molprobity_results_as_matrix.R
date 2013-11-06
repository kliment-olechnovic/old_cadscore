library(gplots);

rt=read.table("./output/decoys/pairs_differences_table.bak", header=TRUE, stringsAsFactors=FALSE);

rt=rt[which(abs(rt$diffs_AA)>0),];
rt=rt[which(abs(rt$diffs_AS)>0),];
rt=rt[which(abs(rt$diffs_SS)>0),];
rt=rt[which(abs(rt$diffs_MM)>0),];
rt=rt[which(abs(rt$diffs_rna_inf_norv)>0),];
rt=rt[which(abs(rt$diffs_rna_rmsd)>0),];
rt=rt[which(abs(rt$diffs_rna_di)>0),];
rt=rt[which(abs(rt$diffs_GDT_C3)>0),];

rt$mp_sign_descriptor=sign(rt$diffs_MP_clashscore);
rt=rt[which(abs(rt$mp_sign_descriptor)==1),];

#########################################

size_rmsd=100;
size_inf=100;

aM=matrix(rep(0, size_rmsd*size_inf), nrow=size_rmsd, ncol=size_inf);
bM=matrix(rep(0, size_rmsd*size_inf), nrow=size_rmsd, ncol=size_inf);

step_rmsd=max(rt$maxs_rna_rmsd)/(size_rmsd-1);
step_inf=max(rt$mins_rna_inf_norv)/(size_inf-1);

row_ids=rt$maxs_rna_rmsd/step_rmsd+1;
col_ids=rt$mins_rna_inf_norv/step_inf+1;

iterators=1:length(row_ids);
for(i in iterators)
{
	a=sign(rt$diffs_AA[i]);
	b=sign(rt$diffs_rna_inf_norv[i]);
	if(a!=b)
	{
		row_id=row_ids[i];
		col_id=col_ids[i];
		if(a==rt$mp_sign_descriptor[i])
		{
			aM[row_id, col_id]=aM[row_id, col_id]+1;
		}
		else
		{
			bM[row_id, col_id]=bM[row_id, col_id]+1;
		}
	}
}

M=matrix(rep(0, size_rmsd*size_inf), nrow=size_rmsd, ncol=size_inf);
for(i in 1:size_rmsd)
{
	for(j in 1:size_inf)
	{
		M[i, j]=sign(aM[i, j]-bM[i, j]);
	}
}

#filled.contour(M);

image(x=1:size_rmsd, y=1:size_inf, z=M, col=colorpanel(3, "#777777", "#FFFFFF", "#000000"), main="mp_clashscore", xlab="RMSD", ylab="INF", axes=FALSE);
axis(1, 1:size_rmsd, labels=round((1:size_rmsd-1)*step_rmsd, 2), las=2)
axis(2, 1:size_inf, labels=round((1:size_inf-1)*step_inf, 2), las=2)
