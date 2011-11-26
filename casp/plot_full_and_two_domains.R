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

  v0=c();
  v1=c();
  v2=c();
  for(j in groups_set)
  {
    group_sel=which(groups==j);
    v0=c(v0, values[group_sel[which(domains[group_sel]==0)]]);
    v1=c(v1, values[group_sel[which(domains[group_sel]==1)]]);
    v2=c(v2, values[group_sel[which(domains[group_sel]==2)]]);
  }

  vt=data.frame(group=groups_set, d0=v0, d1=v1, d2=v2);
  vt=vt[order(vt$d0), ];

  png(paste(cmd_args[2], i, ".png", sep=""), width=12, height=9, units="in", res=300);
  plot(x=c(0, length(groups_set)), y=c(0,1));
  points(x=vt$d0, col="red");
  points(x=vt$d1, col="green");
  points(x=vt$d2, col="blue");
  dev.off();
}
