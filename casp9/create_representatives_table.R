t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$domain>0),];

full_targets=t$target*10+t$domain;

selected_targets=c();
selected_domains=c();
selected_groups=c();
selected_GDT_TS=c();

targets_set=union(full_targets, full_targets);
for(target in targets_set)
{
  st=t[which(full_targets==target),];
  m=mean(st$CASP_GDT_TS);
  distances=abs(st$CASP_GDT_TS-m);
  min_distance=min(distances);
  selection=which(distances==min_distance)[1];
  selected_targets=c(selected_targets, st$target[selection]);
  selected_domains=c(selected_domains, st$domain[selection]);
  selected_groups=c(selected_groups, st$group[selection]);
  selected_GDT_TS=c(selected_GDT_TS, st$CASP_GDT_TS[selection]);
}

representatives_table=data.frame(target=selected_targets, domain=selected_domains, group=selected_groups, GDT_TS=selected_GDT_TS);
write.table(representatives_table, "representatives_table", quote=FALSE, row.names=FALSE);
