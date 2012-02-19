t1=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t1$fullmodel=(t1$target*10+t1$domain)*1000+t1$group;

t2=read.table("casp9_GDT_HA_table", header=TRUE, stringsAsFactors=FALSE);
t2$fullmodel=(t2$target*10+t2$domain)*1000+t2$group;

t=merge(t1, t2, by="fullmodel");
t$CASP_GDT_HA=t$CASP_GDT_HA/100;

output_directory="plot_GDT_HA_correlation_analysis";
dir.create(output_directory);

cadscore_names=c("AA", "AM", "AS", "MA", "MM", "MS", "SA", "SM", "SS", "CASP_GDT_TS");

cor_table=c();

red_spectrum=c("#FF8888", "#FF6666", "#FF4444", "#FF3333", "#FF1111", "#FF0000");

for(score_name in cadscore_names)
{
    x=t$CASP_GDT_HA;
    y=t[, score_name];
    
    png(paste(output_directory, "/", "correlation_", score_name, ".png", sep=""), height=7, width=7, units="in", res=200);
    
    blue_density_colors=densCols(x, y);
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col=blue_density_colors, pch=16, cex=0.5, xlab="GDT_HA", ylab=score_name, main=paste(score_name, " correlation with GDT_HA", sep=""));

    red_density_colors=densCols(x, y, colramp=colorRampPalette(red_spectrum));
    fm_selection=which(t$target_class!="TBM");
    points(x=x[fm_selection], y=y[fm_selection], col=red_density_colors[fm_selection], pch=16, cex=0.5);

    legend(0, 1, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
    
    dev.off();
    
    cor_row=data.frame(score_name=score_name, models_count=length(x), cor_pearson=cor(x, y, method="pearson"), cor_spearman=cor(x, y, method="spearman"));
    if(length(cor_table)==0)
    {
      cor_table=cor_row;
    }
    else
    {
      cor_table=rbind(cor_table, cor_row);
    }
}

write.table(cor_table, "correlation_GDT_HA_table", quote=FALSE, row.names=FALSE);

