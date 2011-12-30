cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$server==1),];
t=t[which(t$solid_group==1),];
t=t[which(t$domain!=0),]
t=t[which(t$target!=629),];
t$GDT_TS=t$GDT_TS/100

filters_info=c("Only server models");
filters_info=c(filters_info, "Only models by solid groups (group is considered to be solid if all its models have big enough atoms/residues ratio)");
filters_info=c(filters_info, "Only domain targets");
filters_info=c(filters_info, "Target 629 is excluded");
write.table(data.frame(filter_description=filters_info), paste(cmd_args[2], "filter_info.txt", sep=""), quote=TRUE, row.names=TRUE);

########################

score_names=c("sm0_AA", "sm0_AM", "sm0_AS", "sm0_AW", "sm0_MA", "sm0_MM", "sm0_MS", "sm0_MW", "sm0_SA", "sm0_SM", "sm0_SS", "sm0_SW");
score_names=c(score_names, "sm1_AA", "sm1_AM", "sm1_AS", "sm1_AW", "sm1_MA", "sm1_MM", "sm1_MS", "sm1_MW", "sm1_SA", "sm1_SM", "sm1_SS", "sm1_SW");
score_names=c(score_names, "sm2_AA", "sm2_AM", "sm2_AS", "sm2_AW", "sm2_MA", "sm2_MM", "sm2_MS", "sm2_MW", "sm2_SA", "sm2_SM", "sm2_SS", "sm2_SW");

cor_table=c();

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
  png(paste(cmd_args[2], target_name, ".png", sep=""), height=4*9, width=4*4, units="in", res=150);
  par(mfrow=c(9, 4));
  for(score_name in score_names)
  {
    x=st$GDT_TS;
    y=st[, score_name];
    coloring="blue";
    if(full_target_id<0) { coloring=densCols(x, y); }
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col=coloring, pch=16, cex=0.5, xlab="GDT_TS", ylab=score_name, main=paste(score_name, " correlation with GDT_TS", sep=""));
    if(full_target_id==0-9)
    {
      red_spectrum=c("#FF8888", "#FF6666", "#FF4444", "#FF3333", "#FF1111", "#FF0000");
      red_density_colors=densCols(x, y, colramp=colorRampPalette(red_spectrum));
      fm_selection=which(st$target_class!="TBM");
      points(x=x[fm_selection], y=y[fm_selection], col=red_density_colors[fm_selection], pch=16, cex=0.5);
    }
    legend(0, 1, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
    
    cor_row=data.frame(target_name=paste(target_name), score_name=score_name, models_count=length(x), cor_pearson=cor(x, y, method="pearson"), cor_spearman=cor(x, y, method="spearman"));
    if(length(cor_table)==0)
    {
      cor_table=cor_row;
    }
    else
    {
      cor_table=rbind(cor_table, cor_row);
    }
  }
  dev.off();
}

write.table(cor_table, paste(cmd_args[2], "cor_table.txt", sep=""), quote=FALSE, row.names=FALSE);
