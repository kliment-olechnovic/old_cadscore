args=commandArgs(trailingOnly=TRUE);
input_table_file=args[1];
output_directory=args[2];

t=read.table(input_table_file, header=TRUE, stringsAsFactors=FALSE);
t$rna_di=t$rna_rmsd/t$rna_inf;

dir.create(output_directory);

cadscore_names=c("AA", "AM", "AS", "MA", "MM", "MS", "SA", "SM", "SS", "na_stacking", "na_stacking_down", "na_stacking_up", "na_siding");
all_refscore_names=c("rna_inf", "rna_inf_norv", "rna_inf_norv_os", "rna_inf_norv_obp", "rna_rmsd", "ext_rmsd", "ext_di_all", "ext_inf_all", "ext_inf_wc", "ext_inf_nwc", "ext_inf_stacking", "TM_score", "TM_score_GDT_TS", "TM_score_GDT_HA");
refscore_names=intersect(all_refscore_names, colnames(t));

cor_table=c();

png(paste(output_directory, "/cor_table.png", sep=""), height=4.8*length(cadscore_names), width=5.0*length(refscore_names), units="in", res=200);
par(mfrow=c(length(cadscore_names), length(refscore_names)));

for(score_name in cadscore_names)
{
for(refscore_name in refscore_names)
{
if(refscore_name!=score_name)
{
  good_sel=which(t[, refscore_name]>=0);
  x=t[good_sel, score_name];
  y=t[good_sel, refscore_name];
  
  cor_pearson=cor(x, y, method="pearson");
  cor_spearman=cor(x, y, method="spearman");
  cor_row=data.frame(ref_score_name=refscore_name, cad_score_name=score_name, models_count=length(x), cor_pearson=cor_pearson, cor_spearman=cor_spearman);
  if(length(cor_table)==0)
  {
    cor_table=cor_row;
  }
  else
  {
    cor_table=rbind(cor_table, cor_row);
  }
  
  x_bounds=c(0, 1);
  y_bounds=c(0, 1);
  if(is.element(refscore_name, c("rna_rmsd", "rna_di", "ext_rmsd", "ext_di_all")))
  {
	  y_bounds=c(0, max(y));
  }
  blue_density_colors=densCols(x, y);
  plot_main_title=paste(score_name, " vs ", refscore_name, "\n", "Pearson c. c. = ", format(cor_pearson, digits=3), ", Spearman c. c. = ", format(cor_spearman, digits=3), sep="");
  plot(x=x, y=y, xlim=x_bounds, ylim=y_bounds, col=blue_density_colors, pch=16, cex=1.0, xlab=score_name, ylab=refscore_name, main=plot_main_title);
}
}
}

dev.off();

cor_table=cor_table[rev(order(cor_table$cor_pearson)),];
write.table(cor_table, paste(output_directory, "/cor_table.txt", sep=""), quote=FALSE, row.names=FALSE);

png(paste(output_directory, "/cor_table__pearson__vs__spearman.png", sep=""), height=4.8, width=5.0, units="in", res=200);
plot(x=cor_table$cor_pearson, y=cor_table$cor_spearman, xlim=c(-1, 1), ylim=c(-1, 1), col="red", xlab="Pearson correlation coefficient", ylab="Spearman correlation coefficient", main="Pearson c. c. vs Spearman c. c.");
points(x=c(-1, 1), y=c(-1, 1), type="l");
dev.off();
