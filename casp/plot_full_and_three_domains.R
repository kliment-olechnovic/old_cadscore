cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE);

targets=t$target[which(t$target_parts_count==4)];
targets_set=union(targets, targets);

for(i in targets_set)
{
  target_sel=which(t$target==i);
  domains=t$domain[target_sel];
  groups=t$group[target_sel];
  values=t$sm0_SA[target_sel];
  gdt=t$GDT_TS[target_sel]/100;
  groups_set=intersect(groups[which(domains==0)], intersect(groups[which(domains==1)], groups[which(domains==2)]));

  good_groups=c();
  v0=c();
  v1=c();
  v2=c();
  v3=c();
  g0=c();
  g1=c();
  g2=c();
  g3=c();
  for(j in groups_set)
  {
    group_sel=which(groups==j);
    check=length(which(domains[group_sel]==0))*length(which(domains[group_sel]==1))*length(which(domains[group_sel]==2))*length(which(domains[group_sel]==3));
    if(check==1)
    {
      good_groups=c(good_groups, j);
      v0=c(v0, values[group_sel[which(domains[group_sel]==0)]]);
      v1=c(v1, values[group_sel[which(domains[group_sel]==1)]]);
      v2=c(v2, values[group_sel[which(domains[group_sel]==2)]]);
      v3=c(v3, values[group_sel[which(domains[group_sel]==3)]]);
      g0=c(g0, gdt[group_sel[which(domains[group_sel]==0)]]);
      g1=c(g1, gdt[group_sel[which(domains[group_sel]==1)]]);
      g2=c(g2, gdt[group_sel[which(domains[group_sel]==2)]]);
      g3=c(g3, gdt[group_sel[which(domains[group_sel]==3)]]);
    }
  }

  vt=data.frame(group=good_groups, d0=v0, d1=v1, d2=v2, d3=v3);
  vt=vt[order(vt$d0), ];

  gt=data.frame(group=good_groups, d0=g0, d1=g1, d2=g2, d3=g3);
  gt=gt[order(gt$d0), ];

  png(paste(cmd_args[2], i, ".png", sep=""), width=12, height=9, units="in", res=300);
  par(mfrow=c(1,2));
  plot(x=c(0, length(good_groups)), y=c(0,1), main=paste("Target ", i, ", GDT_TS of full(red), D1(green), D2(blue), D3(purple)", sep=""), xlab="Group", ylab="GDT_TS");
  points(x=gt$d0, col="red");
  points(x=gt$d1, col="green");
  points(x=gt$d2, col="blue");
  points(x=gt$d3, col="purple");
  plot(x=c(0, length(good_groups)), y=c(0,1), main=paste("Target ", i, ", sm0_SA of full(red), D1(green), D2(blue), D3(purple)", sep=""), xlab="Group", ylab="sm0_SA");
  points(x=vt$d0, col="red");
  points(x=vt$d1, col="green");
  points(x=vt$d2, col="blue");
  points(x=vt$d3, col="purple");
  dev.off();
}
