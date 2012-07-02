t=read.table("pairs_differences_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$MolProbity!=0),];
t=t[which(t$GDT_TS!=0),];
t=t[which(t$SS!=0),];

output_directory="distribution";
dir.create(output_directory);

scores_names=c("MolProbity", "GDT_TS", "AA", "AS", "SS");

for(score_name in scores_names)
{
png(paste(output_directory, "/histogram_of_", score_name, "_differences.png", sep=""), height=5, width=5, units="in", res=200);
hist(t[, score_name], breaks=100, col="red", main="", xlab="", ylab="");
usr=par("usr");
clip(usr[1], (0-sd(t[, score_name])), usr[3], usr[4]);
hist(t[, score_name], breaks=100, col="green", add=TRUE);
clip(sd(t[, score_name]), usr[2], usr[3], usr[4]);
hist(t[, score_name], breaks=100, col="green", add=TRUE);
do.call("clip", as.list(usr))
dev.off();
}
