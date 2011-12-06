cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE);
t=t[which(t$target_parts_count==3),];
targets_set=union(t$target, t$target);

list_of_full_with_two_domains=c();

for(trg in targets_set)
{
  st=t[which(t$target==trg),]
  groups_set=intersect(st$group[which(st$domain==0)], st$group[which(st$domain==1)]);
  groups_set=intersect(groups_set, st$group[which(st$domain==2)]);
  
  for(group in groups_set)
  {
    new_entry=data.frame(target=c(trg), group=c(group));
    if(length(list_of_full_with_two_domains)==0)
    {
      list_of_full_with_two_domains=new_entry;
    }
    else
    {
      list_of_full_with_two_domains=rbind(list_of_full_with_two_domains, new_entry);
    }
  }
}

write.table(list_of_full_with_two_domains, cmd_args[2], quote=FALSE, row.names=FALSE);
