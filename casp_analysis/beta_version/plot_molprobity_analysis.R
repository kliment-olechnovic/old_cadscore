t=read.table("agreement_table", header=TRUE, stringsAsFactors=FALSE);

output_directory="plot_molprobity_analysis";
dir.create(output_directory);

tresholds_GDT_TS=c("GDT_TS_60", "GDT_TS_70", "GDT_TS_80");
for(treshold in tresholds_GDT_TS)
{
  cadscore_names=c("AA", "SA", "SS");
  for(cadscore_name in cadscore_names)
  {
    n00=paste("MP without GDT without ", cadscore_name, sep="");
    n0G=paste("MP and GDT without ", cadscore_name, sep="");
    nC0=paste("MP and ", cadscore_name, " without GDT", sep="");
    nCG=paste("MP and ", cadscore_name, " and GDT", sep="");
    
    v00=t[, treshold][which(t$set_name==n00)];
    v0G=t[, treshold][which(t$set_name==n0G)];
    vC0=t[, treshold][which(t$set_name==nC0)];
    vCG=t[, treshold][which(t$set_name==nCG)];
    
    slices=c(vC0, v0G);
    situations=c(nC0, n0G);
    png(paste(output_directory, "/", "pie1_", cadscore_name, "_treshold_", treshold, ".png", sep=""), height=5, width=10, units="in", res=200);
    pie(slices, labels=situations, col=rainbow(length(slices)), main=paste("Pie for ", cadscore_name, " ", treshold, sep=""));
    dev.off();
    
    slices=c(v00, v0G, vC0, vCG);
    situations=c(n00, n0G, nC0, nCG);
    png(paste(output_directory, "/", "pie2_", cadscore_name, "_treshold_", treshold, ".png", sep=""), height=5, width=10, units="in", res=200);
    pie(slices, labels=situations, col=rainbow(length(slices)), main=paste("Pie for ", cadscore_name, " ", treshold, sep=""));
    dev.off();
  }
}

###########################################

t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$MolProbityScore>(0-1)),];
t=t[which(t$domain>0),];

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
