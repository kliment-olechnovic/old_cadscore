t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$MolProbityScore>(0-1)),];
t=t[which(t$domain>0),];

MolProbity=c();
GDT_TS=c();
min_GDT_TS=c();
AA=c();
SA=c();
SS=c();

targets_set=union(t$target, t$target);
for(target in targets_set)
{
  st=t[which(t$target==target),];
  N=length(st[[1]]);
  for(i in 1:(N-1))
  {
    for(j in (i+1):N)
    {
      min_GDT_TS_val=min(st$CASP_GDT_TS[i], st$CASP_GDT_TS[j]);
      MolProbity_val=st$MolProbityScore[j]-st$MolProbityScore[i];
      GDT_TS_val=st$CASP_GDT_TS[i]-st$CASP_GDT_TS[j];
      AA_val=st$AA[i]-st$AA[j];
      SA_val=st$SA[i]-st$SA[j];
      SS_val=st$SS[i]-st$SS[j];
      if(length(min_GDT_TS_val)*length(MolProbity_val)*length(GDT_TS_val)*length(AA_val)*length(SA_val)*length(SS_val)==1)
      {
        min_GDT_TS=c(min_GDT_TS, min_GDT_TS_val);
        MolProbity=c(MolProbity, MolProbity_val);
        GDT_TS=c(GDT_TS, GDT_TS_val);
        AA=c(AA, AA_val);
        SA=c(SA, SA_val);
        SS=c(SS, SS_val);
      }
    }
  }
}

pairs_differences_table=data.frame(MolProbity=MolProbity, GDT_TS=GDT_TS, min_GDT_TS=min_GDT_TS, AA=AA, SA=SA, SS=SS);
write.table(pairs_differences_table, "pairs_differences_table", quote=FALSE, row.names=FALSE);
