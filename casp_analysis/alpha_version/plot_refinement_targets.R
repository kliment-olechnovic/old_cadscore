cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE);

targets_set=union(t$target, t$target);

ref_score_type="TM_score_GDT_HA";
score_types=c("TM_score_GDT_HA", "sm0_AA", "sm0_SS");

for(i in targets_set)
{
  tt=t[which(t$target==i), ];
  png(paste(cmd_args[2], i, ".png", sep=""), width=5*length(score_types), height=5, units="in", res=200);
  par(mfrow=c(1, length(score_types)));
  for(score_type in score_types)
  {
    scores=tt[, score_type];
    max_score=max(scores);
    best=tt[which(scores==max_score),];
    plot(x=c(0, 1), y=c(0,1), main=paste("Refinement target", i, "-", ref_score_type, "vs", score_type, sep=" "), xlab=ref_score_type, ylab=score_type, type="n");
    points(x=tt[, ref_score_type], y=tt[, score_type], col="blue", pch=21);
    stt=tt[which(tt$group==0), ];
    points(x=stt[, ref_score_type], y=stt[, score_type], col="red", pch=16);
    legend(0.05, 0.95, c("Initial", "Refined"), col=c("red","blue"), pch=c(21,16));
    text(x=0.7, y=0.95, labels=paste("The best is TR", best$target[1], "TS", best$group[1], "_", best$model[1], sep=""));
    text(x=0.7, y=0.9, labels=paste("Best score improved ", sprintf("%.3f", (max_score/stt[, score_type][1])), " times", sep=""));
    text(x=0.7, y=0.85, labels=paste(sprintf("%.2f", length(which(scores>stt[, score_type][1]))/length(scores)*100), "% are better", sep=""));
  }
  dev.off();
}
