t=read.table("shrinking_table", header=TRUE, stringsAsFactors=FALSE);

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
    for(i in length(sst[[1]]))
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
plot(x=factors_set, y=SS_means, type="l", col="red");
points(x=factors_set, y=AA_means, type="l", col="blue");
points(x=factors_set, y=SA_means, type="l", col="purple");
points(x=factors_set, y=GDT_TS_means, type="l", col="green");
