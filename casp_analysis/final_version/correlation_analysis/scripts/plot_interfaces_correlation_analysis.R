t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$interface_AA>=0),];

output_directory="interfaces_correlation_plots";
dir.create(output_directory);

refscore_names=c("interface_AA");
score_names=c("AA", "AM", "AS", "MA", "MM", "MS", "SA", "SM", "SS", "CASP_GDT_TS");

cor_table=c();

for(refscore_name in refscore_names)
{

for(score_name in score_names)
{
    x=t[, refscore_name];
    y=t[, score_name];
    
    png(paste(output_directory, "/", "correlation_", refscore_name, "_vs_", score_name, ".png", sep=""), height=7, width=7, units="in", res=200);
    
    plot(x=x, y=y, xlim=c(0, 1), ylim=c(0, 1), col="black", pch=16, cex=0.5, xlab=refscore_name, ylab=score_name, main=paste(score_name, " correlation with ", refscore_name, sep=""));

    legend(0, 1, c(paste("Pearson k =", format(cor(x, y, method="pearson"), digits=3)), paste("Spearman k =", format(cor(x, y, method="spearman"), digits=3))));
    
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

write.table(cor_table, "interfaces_correlation_table", quote=FALSE, row.names=FALSE);
