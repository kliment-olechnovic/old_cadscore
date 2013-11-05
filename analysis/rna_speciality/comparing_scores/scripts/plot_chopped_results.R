input_table_file="../collecting_scores/output/decoys/calculated_scores_for_chopped";

t=read.table(input_table_file, header=TRUE, stringsAsFactors=FALSE);

models=union(t$model, t$model);

score_names=c("AA", "SS", "rna_inf_norv");

for(score_name in score_names)
{
	plot(x=c(0, max(t$chopped)), y=c(0, 1), type="n", main=paste("Chopped models ",  score_name, " scores", sep=""), xlab="One end choppped percents", ylab="Score");
	
	for(model in models)
	{
		st=t[which(t$model==model),];
		st=st[order(st$chopped),];
		points(x=st$chopped, y=st[,score_name], type="l");
	}
}
