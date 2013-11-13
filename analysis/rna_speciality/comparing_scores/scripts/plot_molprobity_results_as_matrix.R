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

size_rmsd=10;
size_inf=10;

emptyM=matrix(rep(0, size_rmsd*size_inf), nrow=size_rmsd, ncol=size_inf);
bothM=emptyM;
aM=emptyM;
bM=emptyM;
noneM=emptyM;

step_rmsd=max(rt$maxs_rna_rmsd)/(size_rmsd-1);
step_inf=max(rt$mins_rna_inf_norv)/(size_inf-1);

row_ids=floor(rt$maxs_rna_rmsd/step_rmsd)+1;
col_ids=floor(rt$mins_rna_inf_norv/step_inf)+1;

iterators=1:length(row_ids);
for(i in iterators)
{
	a=sign(rt$diffs_AA[i]);
	b=sign(rt$diffs_rna_inf_norv[i]);
	row_id=row_ids[i];
	col_id=col_ids[i];
	if(a==b)
	{
		if(a==rt$mp_sign_descriptor[i])
		{
			bothM[row_id, col_id]=bothM[row_id, col_id]+1;
		}
		else
		{
			noneM[row_id, col_id]=bothM[row_id, col_id]+1;
		}
	}
	else
	{
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

M=emptyM;
for(i in 1:size_rmsd)
{
	for(j in 1:size_inf)
	{
#		both=bothM[i, j];
		a=aM[i, j];
		b=bM[i, j];
#		none=noneM[i, j];
		r=0;
		if(a>b)
		{
			r=1;
		}
		else if(a<b)
		{
			r=2;
		}
#		else if(both>none)
#		{
#			r=3;
#		}
#		else if(none>both)
#		{
#			r=4;
#		}
		M[i, j]=r;
	}
}

image(x=1:size_rmsd, y=1:size_inf, z=M, col=c("#FFFFFF", "#00FF00", "#FF0000"), main="MP", xlab="RMSD", ylab="INF", axes=FALSE);
axis(1, 1:size_rmsd, labels=round(((1:size_rmsd)-1)*step_rmsd, 2), las=2)
axis(2, 1:size_inf, labels=round(((1:size_inf)-1)*step_inf, 2), las=2)
