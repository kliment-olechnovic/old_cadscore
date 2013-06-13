rt=read.table("./output/decoys/molprobity_results.txt", header=TRUE, stringsAsFactors=FALSE);

second_score_names=union(rt$second_score_name, rt$second_score_name);

for(second_score_name in second_score_names)
{
	t=rt[which(rt$second_score_name==second_score_name),];
	subset_names=sort(union(t$subset_name, t$subset_name));
	s1=t[which(t$subset_name==subset_names[1]),];
	s2=t[which(t$subset_name==subset_names[2]),];
	s3=t[which(t$subset_name==subset_names[3]),];
	s4=t[which(t$subset_name==subset_names[4]),];
	par(mar=c(12, 5, 5, 4), xpd=TRUE);
	plot(x=c(0, 1), y=c(0, 1), xlim=c(0, 1), ylim=c(0, 1), type="n", xlab="Minimum RNA_INF in a pair", ylab="Fraction", main=paste("Fractions for CAD-score SS and ", second_score_name, sep=""));
	points(s1$min_RNA_INF, s1$subset_size/s1$set_size, type="l", col="blue");
	points(s1$min_RNA_INF, s2$subset_size/(s2$subset_size+s3$subset_size), type="l", col="green", lwd=3);
	points(s1$min_RNA_INF, s4$subset_size/s1$set_size, type="l", col="red");
	points(s1$min_RNA_INF, s1$set_size/max(s1$set_size), type="l", col="brown");
	legend(0, -0.35, c("Filtered by minimum RNA_INF in a pair", "Both agreed with MP", "CAD-score SS agreed with MP", "Both disagreed with MP"), lwd=c(1, 1, 3, 1), col=c("brown", "blue", "green", "red"));
}
