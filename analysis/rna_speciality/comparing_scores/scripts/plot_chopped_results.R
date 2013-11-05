input_table_file="../collecting_scores/output/decoys/calculated_scores_for_chopped";

t=read.table(input_table_file, header=TRUE, stringsAsFactors=FALSE);

models=union(t$model, t$model);

score_names=c("AA", "SS", "rna_inf_norv");

for(score_name in score_names)
{
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
		}
	}
}
