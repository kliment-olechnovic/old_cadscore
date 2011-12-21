cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$solid_group==1),];
t=t[which(t$domain!=0),]
t=t[which(t$target!=629),];
t$GDT_TS=t$GDT_TS/100

########################

score_names=c("sm0_AA", "sm0_AM", "sm0_AS", "sm0_AW", "sm0_MA", "sm0_MM", "sm0_MS", "sm0_MW", "sm0_SA", "sm0_SM", "sm0_SS", "sm0_SW");

full_target_ids=t$target*10+t$domain;
full_target_ids_set=c(0-89, 0-8, 0-9, union(full_target_ids, full_target_ids));
for(full_target_id in full_target_ids_set)
{
  target_name="all";
  target_sel=1:length(full_target_ids);
  if(full_target_id==0-8)
  {
    target_name="all_casp8";
    target_sel=which(t$CASP==8);
  }
  if(full_target_id==0-9)
  {
    target_name="all_casp9";
    target_sel=which(t$CASP==9);
  }
  if(full_target_id>=0)
  {
    target_name=full_target_id;
    target_sel=which(full_target_ids==full_target_id);
  }
  
  st=t[target_sel,];
  png(paste(cmd_args[2], target_name, ".png", sep=""), height=4*3, width=4*4, units="in", res=150);
  par(mfrow=c(3, 4));
  for(score_name in score_names)
  {
    x=st$GDT_TS;
    y=st[, score_name];
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col=densCols(x, y), pch=16, cex=0.5, xlab="GDT_TS", ylab=score_name, main=paste(score_name, " correlation with GDT_TS", sep=""));
    legend(0, 1, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
  }
  dev.off();
}
