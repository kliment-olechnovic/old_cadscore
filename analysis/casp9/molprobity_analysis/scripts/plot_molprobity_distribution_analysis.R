t=read.table("pairs_differences_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$MolProbity!=0),];
t=t[which(t$GDT_TS!=0),];
t=t[which(t$SS!=0),];

output_directory="distribution";
dir.create(output_directory);

scores_names=c("MolProbity", "GDT_TS", "AA", "AS", "SS");

sds=c();

for(score_name in scores_names)
{
scores_vector=t[, score_name];
sds=c(sds, sd(scores_vector));
png(paste(output_directory, "/histogram_of_", score_name, "_differences.png", sep=""), height=5, width=5, units="in", res=200);
xlim=c(-0.4, 0.4);
if(score_name=="MolProbity") {xlim=c(-4, 4);}
hist(scores_vector, breaks=100, col="red", main="", xlab="", ylab="", xlim=xlim, ylim=c(0, 10000));
usr=par("usr");
clip(usr[1], (0-sd(scores_vector)), usr[3], usr[4]);
hist(scores_vector, breaks=100, col="green", add=TRUE);
clip((0+sd(scores_vector)), usr[2], usr[3], usr[4]);
hist(scores_vector, breaks=100, col="green", add=TRUE);
do.call("clip", as.list(usr))
dev.off();
}

table_sds=data.frame(score_name=scores_names, sd=sds);
write.table(table_sds, paste(output_directory, "/deviations", sep=""), quote=FALSE, row.names=FALSE);
