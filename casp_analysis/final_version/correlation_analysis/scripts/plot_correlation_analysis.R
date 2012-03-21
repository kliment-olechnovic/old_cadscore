t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$domain>0),];

output_directory="correlation_plots";
dir.create(output_directory);

refscore_names=c("CASP_GDT_TS", "CASP_GDT_HA");
cadscore_names=c("AA", "AM", "AS", "MA", "MM", "MS", "SA", "SM", "SS");

cor_table=c();

red_spectrum=c("#FF7777", "#FF6666", "#FF4747", "#FF3333", "#FF1717", "#FF0000");
green_spectrum=c("#77FF77", "#66FF66", "#47FF47", "#33FF33", "#17FF17", "#00FF00");
blue_spectrum=c("#7777FF", "#6666FF", "#4747FF", "#3333FF", "#1717FF", "#0000FF");

for(refscore_name in refscore_names)
{

for(score_name in cadscore_names)
{
    x=t[, refscore_name];
    y=t[, score_name];
    
    red_density_colors=densCols(x, y, colramp=colorRampPalette(red_spectrum));
    green_density_colors=densCols(x, y, colramp=colorRampPalette(green_spectrum));
    blue_density_colors=densCols(x, y, colramp=colorRampPalette(blue_spectrum));
    
    tbm_selection=which(t$target_class=="TBM");
    fm_selection=which(t$target_class=="FM");
    fm_tbm_selection=which(t$target_class=="FM_TBM");
    
    png(paste(output_directory, "/", "correlation_", refscore_name, "_vs_", score_name, ".png", sep=""), height=4.8, width=4.4, units="in", res=300);
    
    plot(x=x[tbm_selection], y=y[tbm_selection], xlim=c(0, 1), ylim=c(0, 1), col=blue_density_colors[tbm_selection], pch=16, cex=0.4, xlab="", ylab="", main="");

    points(x=x[fm_selection], y=y[fm_selection], col=red_density_colors[fm_selection], pch=16, cex=0.4);
    
    points(x=x[fm_tbm_selection], y=y[fm_tbm_selection], col=green_density_colors[fm_tbm_selection], pch=16, cex=0.4);
    
    dev.off();
    
    cor_row=data.frame(ref_score_name=refscore_name, cad_score_name=score_name, models_count=length(x), cor_pearson=cor(x, y, method="pearson"), cor_spearman=cor(x, y, method="spearman"));
    if(length(cor_table)==0)
    {
      cor_table=cor_row;
    }
    else
    {
      cor_table=rbind(cor_table, cor_row);
    }
}

}

write.table(cor_table, "correlation_table", quote=FALSE, row.names=FALSE);
