cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$target_parts_count==3),];
t=t[which(t$solid_group==1),];
t=t[which(t$target!=629),];
t$GDT_TS=t$GDT_TS/100

tid=read.table(cmd_args[2], header=TRUE, stringsAsFactors=FALSE);

########################

tid_keys=tid$target*10000+tid$group*10+tid$domain;
t_keys=t$target*10000+t$group*10+t$domain;
t0_keys=t_keys[which(is.element(t_keys, tid_keys))];
t1_keys=t_keys[which(is.element(t_keys, (t0_keys+1)))];
t2_keys=t_keys[which(is.element(t_keys, (t0_keys+2)))];
t0_keys_set=intersect(t0_keys, intersect(t1_keys-1, t2_keys-2));

tid_keys_sel=which(is.element(tid_keys, (t0_keys_set+0)));
tid_keys=tid_keys[tid_keys_sel];
tid=tid[tid_keys_sel,]
tid=tid[order(tid_keys),];

t0_keys_sel=which(is.element(t_keys, (t0_keys_set+0)));
t0_keys=t_keys[t0_keys_sel];
t0=t[t0_keys_sel,];
t0=t0[order(t0_keys),];

t1_keys_sel=which(is.element(t_keys, (t0_keys_set+1)));
t1_keys=t_keys[t1_keys_sel];
t1=t[t1_keys_sel,];
t1=t1[order(t1_keys),];

t2_keys_sel=which(is.element(t_keys, (t0_keys_set+2)));
t2_keys=t_keys[t2_keys_sel];
t2=t[t2_keys_sel,];
t2=t2[order(t2_keys),];

########################

model_orientation_error=tid[,"sm0_AA_diff"];
orientation_score=tid[,"sm0_AA"];
normalised_orientation_error=model_orientation_error/t0[,"sm0_AA_ref"];
target_orientation_value=tid[,"sm0_AA_ref"]/t0[,"sm0_AA_ref"];

score_names=c("GDT_TS", "sm0_AA", "sm0_SA", "sm0_SS");
score_colors=c("red", "blue", "purple", "brown");

########################

targets_set=c(0-89, 0-8, 0-9);
targets_set=c(targets_set, union(t0$target, t0$target));
for(target in targets_set)
{
  target_name="all";
  target_sel=1:length(t0$target);
  if(target==0-8)
  {
    target_name="all_casp8";
    target_sel=which(t0$CASP==8);
  }
  if(target==0-9)
  {
    target_name="all_casp9";
    target_sel=which(t0$CASP==9);
  }
  if(target>=0)
  {
    target_name=target;
    target_sel=which(t0$target==target);
  }
  st0=t0[target_sel,];
  st1=t1[target_sel,];
  st2=t2[target_sel,];
  sorientation_score=orientation_score[target_sel];
  snormalised_orientation_error=normalised_orientation_error[target_sel];
  png(paste(cmd_args[3], target_name, ".png", sep=""), width=4*12, height=4*length(score_names), units="in", res=150);
  par(mfrow=c(length(score_names), 12));
  for(score_name in score_names)
  {
    score_color=score_colors[which(score_names==score_name)];
    
    score_of_both_domains=((st1[, score_name])*(st1$target_atoms_count)+(st2[, score_name])*(st2$target_atoms_count))/(st1$target_atoms_count+st2$target_atoms_count);
    score_diff_between_full_and_domains=(st0[, score_name]-score_of_both_domains);
    score_insiding_value=abs(st1[, score_name]-st2[, score_name])-(abs(st0[, score_name]-st1[, score_name])+abs(st0[, score_name]-st2[, score_name]));
    score_difference_with_lower_value=st0[, score_name]-pmin(st1[, score_name], st2[, score_name]);

    x=st0[, score_name];
    y=score_of_both_domains;
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col=densCols(x, y), pch=16, cex=0.5, xlab="Full score", ylab="Domains score (combined score of two domains)", main=paste(score_name, ": Domains score vs full score", sep=""));
    points(x=c(0, 1), y=c(0, 1), type="l", col="black");
    
    hist(score_of_both_domains/st0[, score_name], xlim=c(0.9, 2.5), col="grey", xlab="(Domains score)/(Full score)", main="Histogram of (Domains score)/(Full score)");
    
    x=sorientation_score;
    y=st0[, score_name];
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col=densCols(x, y), pch=16, cex=0.5, xlab="Orientation AA score", ylab="Full score", main=paste(score_name, ": Full score vs orientation AA score", sep=""));
    legend(0, 1, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));

    x=sorientation_score;
    y=score_diff_between_full_and_domains;
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0-0.25, 0.25), col=densCols(x, y), pch=16, cex=0.5, xlab="Orientation AA score", ylab="Full and domains scores absolute difference", main=paste(score_name, ": Full and domains scores absolute difference", sep=""));
    points(x=c(0, 1), y=c(0, 0), type="l", col="black");
    legend(0, 0.25, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
    
    x=snormalised_orientation_error;
    y=st0[, score_name];
    plot(x=x, y=y, xlim=c(0, 0.2), ylim=c(0, 1), col=densCols(x, y), pch=16, cex=0.5, xlab="Norm. orientation error", ylab="Full score", main=paste(score_name, ": Full score vs orientation error", sep=""));
   
    x=snormalised_orientation_error;
    y=score_diff_between_full_and_domains;
    plot(x=x, y=y, xlim=c(0, 0.2), ylim=c(0-0.25, 0.25), col=densCols(x, y), pch=16, cex=0.5, xlab="Norm. orientation error", ylab="Full and domains scores absolute difference", main=paste(score_name, ": Full and domains scores absolute difference", sep=""));
    points(x=c(0, 1), y=c(0, 0), type="l", col="black");
    legend(0, 0.25, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
    
    x=snormalised_orientation_error;
    y=score_insiding_value;
    plot(x=x, y=y, xlim=c(0, 0.2), ylim=c(-0.25, 0.25), col=densCols(x, y), pch=16, cex=0.5, xlab="Norm. orientation error", ylab="Insiding value (any non-zero means outside)", main=paste(score_name, ": Insiding value", sep=""));
    points(x=c(0, 1), y=c(0, 0), type="l", col="black");
    
    x=snormalised_orientation_error;
    y=score_difference_with_lower_value;
    plot(x=x, y=y, xlim=c(0, 0.2), ylim=c(-0.25, 0.25), col=densCols(x, y), pch=16, cex=0.5, xlab="Norm. orientation error", ylab="Difference between full score and worst domain score", main=paste(score_name, ": Difference between full score and worst domain score", sep=""));
    points(x=c(0, 1), y=c(0, 0), type="l", col="black");
    
    ordering=order(st0[, score_name]);
    plot(st0[, score_name][ordering], col="red", pch=21, cex=0.3, ylim=c(0, 1), xlab="Models oredered by full scores", ylab="Score", main=paste(score_name, ": Full and domains scores", sep=""));
    points(st1[, score_name][ordering], col="blue", pch=21, cex=0.3);
    points(st2[, score_name][ordering], col="green", pch=21, cex=0.3);
    points(st0[, score_name][ordering], col="red", pch=21, cex=0.3);
    legend(0, 1, c("Full", "D1", "D2"), col=c("red", "blue", "green"), pch=rep(21, 21, 21));
    
    x=st0$GDT_TS;
    y=st0[, score_name];
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col=densCols(x, y), pch=16, cex=0.5, xlab="GDT_TS", ylab=score_name, main=paste(score_name, ": Correlation of full score with full GDT_TS", sep=""));
    legend(0, 1, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
    
    x=st1$GDT_TS;
    y=st1[, score_name];
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col=densCols(x, y), pch=16, cex=0.5, xlab="GDT_TS", ylab=score_name, main=paste(score_name, ": Correlation of D1 score with D1 GDT_TS", sep=""));
    legend(0, 1, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
    
    x=st2$GDT_TS;
    y=st2[, score_name];
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col=densCols(x, y), pch=16, cex=0.5, xlab="GDT_TS", ylab=score_name, main=paste(score_name, ": Correlation of D2 score with D2 GDT_TS", sep=""));
    legend(0, 1, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
  }
  dev.off();
}

warnings();
