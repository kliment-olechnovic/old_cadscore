cmd_args=commandArgs(TRUE);

score_names=c("GDT_TS", "sm0_SA");
domains_colors=c("red", "green", "blue", "purple", "magenta", "orange", "skyblue");

t=read.table(cmd_args[1], header=TRUE);
t$GDT_TS=t$GDT_TS/100;

parted_targets=t$target[which(t$target_parts_count>2)];
targets_set=union(parted_targets, parted_targets);

for(trg in targets_set)
{
  st=t[which(t$target==trg),]
  parts_count=st$target_parts_count[1];

  groups_set=intersect(st$group[which(st$domain==0)], st$group[which(st$domain==1)])
  for(part in 2:(parts_count-1))
  {
    groups_set=intersect(groups_set, st$group[which(st$domain==part)]);
  }

  png(paste(cmd_args[2], trg, ".png", sep=""), width=7*length(score_names), height=7, units="in", res=100);
  par(mfrow=c(1, length(score_names)));

  for(score_name in score_names)
  {
    plot(x=c(1, length(groups_set)), y=c(0, 1), type="n", main=paste("Target", trg, "full/domain", score_name, sep=" "), xlab="Group", ylab=score_name);
    group_ordering=data.frame(group_name=st$group[which(st$domain==0)], scores=st[, score_name][which(st$domain==0)]);
    group_ordering=group_ordering[order(group_ordering$scores),];
    for(group in groups_set)
    {
      gt=st[which(st$group==group),];
      check=1;
      for(part in 0:(parts_count-1))
      {
	check=check*length(which(gt$domain==part));
      }
      if(check==1)
      {
	group_num=which(group_ordering$group_name==group)[1];
	for(part in 0:(parts_count-1))
	{
	  points(x=group_num, y=gt[, score_name][which(gt$domain==part)][1], col=domains_colors[part+1], pch=21, cex=0.5);
	}
	if(parts_count==3)
	{
	  points(x=rep(group_num, 2), y=c(gt[, score_name][which(gt$domain==0)][1], gt[, score_name][which(gt$domain==1)][1]), col=domains_colors[2], type="l");
	  points(x=rep(group_num, 2), y=c(gt[, score_name][which(gt$domain==0)][1], gt[, score_name][which(gt$domain==2)][1]), col=domains_colors[3], type="l");
	}
      }
    }
    point_names=c("Full");
    for(part in 1:(parts_count-1))
    {
      point_names=c(point_names, paste("D", part, sep=""));
    }
    legend(0, 1, point_names, col=domains_colors[1:parts_count], pch=rep(21, parts_count));
  }

  dev.off();
}
