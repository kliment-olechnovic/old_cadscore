t=read.table("../collecting_scores/output/rp/calculated_scores_for_chopped", header=TRUE, stringsAsFactors=FALSE);

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
	
	plot(x=c(0, 1), y=c(0, 1), type="n", main=paste("Chopped models ",  score_name, " scores fractions", sep=""), xlab="Model size fraction", ylab="Score fraction");
	
	for(model in models)
	{
		st=t[which(t$model==model),];
		st=st[order(st$chopped),];
		points(x=(1-(st$chopped*2/100)), y=(st[,score_name]/st[which(st$chopped==0),score_name]), type="l", col="black", lwd=0.1);
	}
	
	points(c(0, 1), c(0, 1), col="green", type="l");
}

length(models);
score_names
mishandles

###############################################

t$size_fraction=(1-(t$chopped*2/100));

score_names=c("SS", "rna_inf_norv");
score_cols=c(rgb(0, 0.8, 0.8, 0.8), rgb(0.8, 0, 0.8, 0.8));

size_fraction_interval=c(min(t$size_fraction), max(t$size_fraction));

png("rp_model_size_and_score_fractions.png", height=5.0, width=5.0, units="in", res=600);

plot(x=size_fraction_interval, y=c(0, 1), type="n", main=paste("Model size and score fractions", sep=""), xlab="Model size fraction", ylab="Model score fraction");

for(i in 1:length(score_names))
{
	score_name=score_names[i];
	score_col=score_cols[i];
	
	for(model in models)
	{
		st=t[which(t$model==model),];
		st=st[order(st$chopped),];
		st$score_fraction=(st[,score_name]/st[which(st$chopped==0),score_name]);
		points(x=st$size_fraction, y=st$score_fraction, type="l", col=score_col, lwd=0.3);
	}
}

points(size_fraction_interval, size_fraction_interval, col="black", type="l", lwd=2);

legend("bottomright", c("CAD-score SS", "INF"), lwd=c(2, 2), col=score_cols);

dev.off();
