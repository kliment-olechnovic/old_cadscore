t=read.table("../collecting_scores/output/rp/calculated_scores_for_chopped", header=TRUE, stringsAsFactors=FALSE);

t$rna_di=(t$rna_rmsd/t$rna_inf_norv);
t$rna_rmsd_inv=0.0-t$rna_rmsd;
t$rna_di_inv=0.0-t$rna_di;

models=union(t$model, t$model);

score_names=c("AA", "SS", "TM_score", "rna_inf_norv", "rna_rmsd_inv", "rna_di_inv");

monitored_score_name_v=c();
monitored_chopped_v=c();

for(score_name in score_names)
{
	plot(x=c(0, max(t$chopped)), y=c(min(t[,score_name]), max(t[,score_name])), type="n", main=paste("Chopped models ",  score_name, " scores", sep=""), xlab="One end chopped percents", ylab="Score");
	
	for(model in models)
	{
		st=t[which(t$model==model),];
		st=st[order(st$chopped),];
		points(x=st$chopped, y=st[,score_name], type="l", col="black", lwd=0.1);
	}

	plot(x=c(0, max(t$chopped)), y=c(min(t[,score_name]), max(t[,score_name])), type="n", main=paste("Chopped models badly adjusted ",  score_name, " scores", sep=""), xlab="One end chopped percents", ylab="Score");
	
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
					monitored_score_name_v=c(monitored_score_name_v, score_name);
					monitored_chopped_v=c(monitored_chopped_v, st$chopped[j]);
				}
				points(x=st$chopped[c(i,j)], y=st[c(i,j),score_name], type="l", col=col, lwd=lwd);
			}
		}
	}
}

length(models);

monitored_chopped_union=sort(union(monitored_chopped_v, monitored_chopped_v));
monitored_score_name_union=union(score_names, monitored_score_name_v);
report_t=data.frame(one_end_chopped_pct=monitored_chopped_union);
for(monitored_score_name in monitored_score_name_union)
{
	counts=rep(0, length(monitored_chopped_union))
	for(monitored_chopped in monitored_chopped_union)
	{
		i=which(monitored_chopped_union==monitored_chopped)[1];
		counts[i]=length(intersect(which(monitored_chopped_v==monitored_chopped), which(monitored_score_name_v==monitored_score_name)));
	}
	report_t[,monitored_score_name]=counts;
}
write.table(report_t, "./output/rp/chopping_mishandles.txt", quote=FALSE, row.names=FALSE);

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
