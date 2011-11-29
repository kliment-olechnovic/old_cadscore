cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE);

targets_set=union(t$target, t$target);

score_types=c("sm0_AA", "sm0_SS");

for(i in targets_set)
{
  tt=t[which(t$target==i), ];
  png(paste(cmd_args[2], i, ".png", sep=""), width=5*length(score_types), height=5, units="in", res=200);
  par(mfrow=c(1, length(score_types)));
  for(score_type in score_types)
  {
    plot(x=c(0, 1), y=c(0,1), main=paste("Refinement target ", i, " GDT_HA vs ", score_type, sep=""), xlab="GDT_HA", ylab=score_type, type="n");
    points(x=tt$TM_score_GDT_HA, y=tt[, score_type], col="blue", pch=21);
    stt=tt[which(tt$group==0), ];
    points(x=stt$TM_score_GDT_HA, y=stt[, score_type], col="red", pch=16);
    legend(0.1, 0.9, c("Initial", "Refined"), col=c("red","blue"), pch=c(21,16));
  }
  dev.off();
}
