t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$domain>0),];
t=t[which(t$CASP_GDT_TS>0.8),];

full_targets=t$target*10+t$domain;

selected_targets=c();
selected_domains=c();
selected_groups=c();
selected_GDT_TS=c();

targets_set=union(full_targets, full_targets);
for(target in targets_set)
{
  st=t[which(full_targets==target),];
  m=max(st$CASP_GDT_TS);
  selection=which(st$CASP_GDT_TS==m)[1];
  selected_targets=c(selected_targets, st$target[selection]);
  selected_domains=c(selected_domains, st$domain[selection]);
  selected_groups=c(selected_groups, st$group[selection]);
  selected_GDT_TS=c(selected_GDT_TS, st$CASP_GDT_TS[selection]);
}

selected_groups_titles=c()
for(group in selected_groups)
{
  prefix="";
  if(group<10)
  {
    prefix="00";
  }
  else if(group<100)
  {
    prefix="0";
  }
  selected_groups_titles=c(selected_groups_titles, paste(prefix, group, sep=""));
}

representatives_table=data.frame(target=selected_targets, domain=selected_domains, group=selected_groups_titles, GDT_TS=selected_GDT_TS);
write.table(representatives_table, "best_representatives_table", quote=FALSE, row.names=FALSE);
