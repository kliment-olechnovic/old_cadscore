working_directory=commandArgs(TRUE)[1];

t=read.table(paste(working_directory, "/agreement_table", sep=""), header=TRUE, stringsAsFactors=FALSE);

tresholds_GDT_TS=c("GDT_TS_60", "GDT_TS_70", "GDT_TS_80");
for(treshold in tresholds_GDT_TS)
{
  cadscore_names=c("AA", "AS", "SS");
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
    png(paste(working_directory, "/", "pie1_", cadscore_name, "_treshold_", treshold, ".png", sep=""), height=5, width=10, units="in", res=200);
    pie(slices, labels=situations, col=rainbow(length(slices)), main=paste("Pie for ", cadscore_name, " ", treshold, sep=""));
    dev.off();
    
    slices=c(v00, v0G, vC0, vCG);
    situations=c(n00, n0G, nC0, nCG);
    png(paste(working_directory, "/", "pie2_", cadscore_name, "_treshold_", treshold, ".png", sep=""), height=5, width=10, units="in", res=200);
    pie(slices, labels=situations, col=rainbow(length(slices)), main=paste("Pie for ", cadscore_name, " ", treshold, sep=""));
    dev.off();
  }
}

