t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$MolProbityScore>(0-1)),];
t=t[which(t$domain>0),];

full_targets=t$target*10+t$domain;

target_names=c();
group_1_names=c();
group_2_names=c();
min_GDT_TS=c();
MolProbity=c();
GDT_TS=c();
AA=c();
AS=c();
SS=c();

targets_set=union(full_targets, full_targets);
for(target in targets_set)
{
  st=t[which(full_targets==target),];
  N=length(st[[1]]);
  for(i in 1:(N-1))
  {
    for(j in (i+1):N)
    {
      min_GDT_TS_val=min(st$CASP_GDT_TS[i], st$CASP_GDT_TS[j]);
      MolProbity_val=st$MolProbityScore[j]-st$MolProbityScore[i];
      GDT_TS_val=st$CASP_GDT_TS[i]-st$CASP_GDT_TS[j];
      AA_val=st$AA[i]-st$AA[j];
      AS_val=st$AS[i]-st$AS[j];
      SS_val=st$SS[i]-st$SS[j];
      if(length(min_GDT_TS_val)*length(MolProbity_val)*length(GDT_TS_val)*length(AA_val)*length(AS_val)*length(SS_val)==1)
      {
        target_names=c(target_names, target);
        group_1_names=c(group_1_names, st$group[i]);
        group_2_names=c(group_2_names, st$group[j]);
        min_GDT_TS=c(min_GDT_TS, min_GDT_TS_val);
        MolProbity=c(MolProbity, MolProbity_val);
        GDT_TS=c(GDT_TS, GDT_TS_val);
        AA=c(AA, AA_val);
        AS=c(AS, AS_val);
        SS=c(SS, SS_val);
      }
    }
  }
}

pairs_differences_table=data.frame(target_name=target_names, group_1_name=group_1_names, group_2_name=group_2_names, MolProbity=MolProbity, GDT_TS=GDT_TS, min_GDT_TS=min_GDT_TS, AA=AA, AS=AS, SS=SS);
write.table(pairs_differences_table, "pairs_differences_table", quote=FALSE, row.names=FALSE);
