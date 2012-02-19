t=read.table("shrinking_models_table", header=TRUE, stringsAsFactors=FALSE);
t$LGA_GDT_TS=t$LGA_GDT_TS/100;

factors=c();
GDT_TS=c();
AA=c();
SA=c();
SS=c();

tagets_set=union(t$target, t$target);
for(target in tagets_set)
{
  st=t[which(t$target==target),];
  models_set=union(st$model, st$model);
  for(model in models_set)
  {
    sst=st[which(st$model==model),];
    nt=sst[which(sst$shrinking==100),];
    for(i in 1:length(sst[[1]]))
    {
      factors=c(factors, sst$shrinking[i]);
      GDT_TS=c(GDT_TS, (sst$LGA_GDT_TS[i]-nt$LGA_GDT_TS[1]));
      AA=c(AA, (sst$AA[i]-nt$AA[1]));
      SA=c(SA, (sst$SA[i]-nt$SA[1]));
      SS=c(SS, (sst$SS[i]-nt$SS[1]));
    }
  }
}

factors_set=sort(union(factors, factors));
GDT_TS_means=c();
AA_means=c();
SA_means=c();
SS_means=c();
for(factor in factors_set)
{
  GDT_TS_means=c(GDT_TS_means, mean(GDT_TS[which(factors==factor)]));
  AA_means=c(AA_means, mean(AA[which(factors==factor)]));
  SA_means=c(SA_means, mean(SA[which(factors==factor)]));
  SS_means=c(SS_means, mean(SS[which(factors==factor)]));
}

output_directory="plot_shrinking_analysis";
dir.create(output_directory);

png(paste(output_directory, "/shrinking_models", ".png", sep=""), height=7, width=7, units="in", res=200);
plot(xlim=c(90, 110), ylim=c(-0.06, 0.04), x=c(0, 1000), y=c(0, 0), type="l", col="black", xlab="Factor %", ylab="Scores didderence", main="Scores didderences");
points(x=factors_set, y=GDT_TS_means, type="l", col="green", lwd=2);
points(x=factors_set, y=AA_means, type="l", col="blue", lwd=2);
points(x=factors_set, y=SA_means, type="l", col="purple", lwd=2);
points(x=factors_set, y=SS_means, type="l", col="red", lwd=2);
legend(100, 0.04, c("LGA GDT_TS", "AA", "SA", "SS"), col=c("green", "blue", "purple", "red"), lty=c(1, 1, 1, 1), lwd=c(2, 2, 2, 2));
dev.off();