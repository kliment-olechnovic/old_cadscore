normal_scores_t=read.table("../collecting_scores/output/decoys/merged_scores", header=TRUE, stringsAsFactors=FALSE);

normal_scores_t=normal_scores_t[which(normal_scores_t$m_res_used/normal_scores_t$t_res_used>0.99),];
allowed_targets=union(normal_scores_t$target, normal_scores_t$target);

t=read.table("../collecting_scores/output/decoys/calculated_scores_for_chopped", header=TRUE, stringsAsFactors=FALSE);

t=t[which(is.element(t$target, allowed_targets)),];

models=union(t$model, t$model);

score_names=c("AA", "SS", "TM_score", "rna_inf_norv");
mishandles=rep(0, length(score_names));

for(score_name in score_names)
{
	plot(x=c(0, max(t$chopped)), y=c(0, 1), type="n", main=paste("Chopped models ",  score_name, " scores", sep=""), xlab="One end chopped percents", ylab="Score");
	
	for(model in models)
	{
		st=t[which(t$model==model),];
		st=st[order(st$chopped),];
		points(x=st$chopped, y=st[,score_name], type="l", col="black", lwd=0.1);
	}

	plot(x=c(0, max(t$chopped)), y=c(0, 1), type="n", main=paste("Chopped models badly adjusted ",  score_name, " scores", sep=""), xlab="One end chopped percents", ylab="Score");
	
	for(model in models)
	{
		st=t[which(t$model==model),];
		st=st[order(st$chopped),];
		n=length(st[[1]]);
		broken=FALSE;
		for(i in 1:(n-1))
		{
			j=i+1;
			if(st[i,score_name]<st[j,score_name])
			{
				broken=TRUE
			}
		}
		if(broken==TRUE)
		{
			for(i in 1:(n-1))
			{
				j=i+1;
				col="blue";
				lwd=0.1;
				if(st[i,score_name]<st[j,score_name])
				{
					col="red";
					lwd=2.0;
				}
				points(x=st$chopped[c(i,j)], y=st[c(i,j),score_name], type="l", col=col, lwd=lwd);
			}
			mishandles[which(score_names==score_name)]=mishandles[which(score_names==score_name)]+1;
		}
	}
}

length(models);
score_names
mishandles
