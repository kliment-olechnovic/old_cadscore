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

g_orientation_value=tid[,"sm0_AA_ref"]/t0[,"sm0_AA_ref"];

score_names=c("GDT_TS", "sm0_AA", "sm0_SA", "sm0_SS");

########################

summary_table=c();

targets_set=c(0-89, 0-8, 0-9);
#targets_set=c(targets_set, union(t0$target, t0$target));
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
  s_orientation_value=g_orientation_value[target_sel];
  png(paste(cmd_args[3], target_name, ".png", sep=""), height=4*4, width=4*length(score_names), units="in", res=150);
  par(mfcol=c(4, length(score_names)));
  for(score_name in score_names)
  {    
    score_of_both_domains=((st1[, score_name])*(st1$target_atoms_count)+(st2[, score_name])*(st2$target_atoms_count))/(st1$target_atoms_count+st2$target_atoms_count);
    score_diff_between_full_and_domains=(st0[, score_name]-score_of_both_domains);
    score_insiding_value=abs(st1[, score_name]-st2[, score_name])-(abs(st0[, score_name]-st1[, score_name])+abs(st0[, score_name]-st2[, score_name]));
    score_difference_with_lower_value=st0[, score_name]-pmin(st1[, score_name], st2[, score_name]);
    
    x=s_orientation_value;
    y=st0[, score_name];
    coloring="blue";
    if(target<0) { coloring=densCols(x, y); }
    plot(x=x, y=y, col=coloring, pch=16, cex=0.5, xlab="Orientation value", ylab="Full score", main=paste(score_name, ": Full score vs orientation value", sep=""));
   
    x=s_orientation_value;
    y=score_diff_between_full_and_domains;
    coloring="blue";
    if(target<0) { coloring=densCols(x, y); }
    plot(x=x, y=y, col=coloring, pch=16, cex=0.5, xlab="Orientation value", ylab="Full and domains scores absolute difference", main=paste(score_name, ": Full and domains scores absolute difference", sep=""));
    points(x=c(0, 1), y=c(0, 0), type="l", col="black");
    legend(0, 0.25, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
    
    
    x=s_orientation_value;
    y=score_insiding_value;
    coloring="blue";
    if(target<0) { coloring=densCols(x, y); }
    plot(x=x, y=y, col=coloring, pch=16, cex=0.5, xlab="Orientation value", ylab="Insiding value (any non-zero means outside)", main=paste(score_name, ": Insiding value", sep=""));
    points(x=c(0, 1), y=c(0, 0), type="l", col="black");
    
    x=s_orientation_value;
    y=score_difference_with_lower_value;
    coloring="blue";
    if(target<0) { coloring=densCols(x, y); }
    plot(x=x, y=y, col=coloring, pch=16, cex=0.5, xlab="Orientation value", ylab="Difference between full score and worst domain score", main=paste(score_name, ": Difference between full score and worst domain score", sep=""));    points(x=c(0, 1), y=c(0, 0), type="l", col="black");
  }
  dev.off();
  
  summary_row=data.frame(target_name=paste(target_name), models_count=length(target_sel));
  if(length(summary_table)==0)
  {
    summary_table=summary_row;
  }
  else
  {
    summary_table=rbind(summary_table, summary_row);
  }
}

write.table(summary_table, paste(cmd_args[3], "summary_table.txt", sep=""), quote=FALSE, row.names=FALSE);

warnings();
