t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$domain>0),];
t=t[which(t$CASP_GDT_TS>0.4),];
t=t[which(t$CASP_GDT_TS<0.8),];

selected_groups=c();
selected_GDT_TS=c();

targets_set=union(t$target, t$target);
for(target in targets_set)
{
  st=t[which(t$target==target),];
  m=mean(st$CASP_GDT_TS);
  distances=abs(st$CASP_GDT_TS-m);
  min_distance=min(distances);
  selection=which(distances==min_distance)[1];
  selected_groups=c(selected_groups, st$group[selection]);
  selected_GDT_TS=c(selected_GDT_TS, st$CASP_GDT_TS[selection]);
}

representatives_table=data.frame(target=targets_set, group=selected_groups, GDT_TS=selected_GDT_TS);
write.table(representatives_table, "representatives_table", quote=FALSE, row.names=FALSE);
