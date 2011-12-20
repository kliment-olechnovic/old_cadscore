cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$target_parts_count==3),];
t=t[which(t$solid_group==1),];
t=t[which(t$target!=629),];
t$GDT_TS=t$GDT_TS/100

tid=read.table(cmd_args[2], header=TRUE, stringsAsFactors=FALSE);

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

orientation_error=tid[,"sm0_AA_diff"];
normalised_orientation_error=orientation_error/t0[,"sm0_AA_ref"];

score_names=c("GDT_TS", "TM_score", "sm0_AA", "sm0_SA", "sm0_SS");
score_colors=c("red", "pink", "blue", "green", "purple");

png(paste(cmd_args[3], "full_vs_domains.png", sep=""), width=4*4, height=4*length(score_names), units="in", res=200);
par(mfrow=c(length(score_names), 4));
for(score_name in score_names)
{
  score_color=score_colors[which(score_names==score_name)];
  
  score_of_both_domains=((t1[, score_name])*(t1$target_atoms_count)+(t2[, score_name])*(t2$target_atoms_count))/(t1$target_atoms_count+t2$target_atoms_count);
  score_diff_between_full_and_domains=(t0[, score_name]-score_of_both_domains);
  score_insiding_value=abs(t1[, score_name]-t2[, score_name])-(abs(t0[, score_name]-t1[, score_name])+abs(t0[, score_name]-t2[, score_name]));
  score_difference_with_lower_value=t0[, score_name]-pmin(t1[, score_name], t2[, score_name]);

  plot(x=t0[, score_name], y=score_of_both_domains, xlim=c(0, 1), ylim=c(0, 1), col=score_color, cex=0.5, xlab="Full score", ylab="Domains score (combined score of two domains)", main=score_name);
  plot(x=normalised_orientation_error, y=score_diff_between_full_and_domains, col=score_color, cex=0.5, xlab="Norm. orientation error", ylab="Full and domains scores absolute difference", main=score_name);
  plot(x=normalised_orientation_error, y=score_insiding_value, col=score_color, cex=0.5, xlab="Norm. orientation error", ylab="Insiding value (any non-zero means outside)", main=score_name);
  plot(x=normalised_orientation_error, y=score_difference_with_lower_value, col=score_color, cex=0.5, xlab="Norm. orientation error", ylab="Difference between full score and worst domain score", main=score_name);
}
dev.off();
