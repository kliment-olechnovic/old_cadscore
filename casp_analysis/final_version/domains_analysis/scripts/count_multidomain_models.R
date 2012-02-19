t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);

multidomain_targets=0;
multidomain_models=0;

second_domains_set=t$target[which(t$domain==2)];
targets_set=union(second_domains_set, second_domains_set);

for(target in targets_set)
{
  st=t[which(t$target==target),];
  domains_set=sort(union(st$domain, st$domain));
  if(length(domains_set)>2)
  {
    multidomain_targets=multidomain_targets+1;
    groups_set=sort(union(st$group, st$group));
    for(g in groups_set)
    {
      stg=st[which(st$group==g),];
      if(length(union(stg$domain, stg$domain))==length(domains_set))
      {
        multidomain_models=multidomain_models+1;
      }
    }
  }
}

result=data.frame(names=c("multidomain_targets", "multidomain_models"), numbers=c(multidomain_targets, multidomain_models));

write.table(result, "multidomain_models_counts", quote=FALSE, row.names=FALSE, col.names=FALSE);
