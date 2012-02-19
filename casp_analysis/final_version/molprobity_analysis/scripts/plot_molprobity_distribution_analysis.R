t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$MolProbityScore>(0-1)),];
t=t[which(t$domain>0),];

output_directory="distribution";
dir.create(output_directory);

score_names=c("CASP_GDT_TS", "AA", "SA", "SS");
for(score_name in score_names)
{
  x=t[, score_name];
  y=t$MolProbityScore;
  blue_density_colors=densCols(x, y);
  png(paste(output_directory, "/", "correlation_", score_name, "_with_MolProbity", ".png", sep=""), height=5, width=5, units="in", res=200);
  plot(x=x, y=y, xlim=c(0, 1), col=blue_density_colors, pch=16, cex=0.5, xlab=score_name, ylab="MolProbityScore", main=paste(score_name, " correlation with MolProbity", sep=""));
  dev.off();
}

png(paste(output_directory, "/histogram_of_MolProbity.png", sep=""), height=5, width=5, units="in", res=200);
hist(t$MolProbityScore);
dev.off();
