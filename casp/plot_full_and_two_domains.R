cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE);

targets=t$target[which(t$target_parts_count==3)];
targets_set=union(targets, targets);

for(i in targets_set)
{
  target_sel=which(t$target==i);
  domains=t$domain[target_sel];
  groups=t$group[target_sel];
  values=t$sm0_SA[target_sel];
  groups_set=intersect(groups[which(domains==0)], intersect(groups[which(domains==1)], groups[which(domains==2)]));
  png(paste(cmd_args[2], i, ".png", sep=""), width=12, height=9, units="in", res=300);
  plot(x=c(0, length(groups_set)), y=c(0,1));
  counter=0;
  for(j in groups_set)
  {
    group_sel=which(groups==j);
    v0=values[group_sel[which(domains[group_sel]==0)]];
    v1=values[group_sel[which(domains[group_sel]==1)]];
    v2=values[group_sel[which(domains[group_sel]==2)]];
    points(x=counter, y=v0, col="red");
    points(x=counter, y=v1, col="green");
    points(x=counter, y=v2, col="blue");
    counter=counter+1;
  }
  dev.off();
}
