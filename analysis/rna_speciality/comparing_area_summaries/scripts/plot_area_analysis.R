args=commandArgs(trailingOnly=TRUE);
input_table_file=args[1];
category_name=args[2];

t=read.table(input_table_file, header=FALSE, stringsAsFactors=FALSE);

names=t[which(t[[2]]=="all_AA"),1];
names=intersect(names, t[which(t[[2]]=="all_MM"),1]);
names=intersect(names, t[which(t[[2]]=="all_SM"),1]);
names=intersect(names, t[which(t[[2]]=="all_SS"),1]);
t=t[which(is.element(t[[1]], names)),];

AA=t[which(t[[2]]=="all_AA"),3];
MM=t[which(t[[2]]=="all_MM"),3];
SM=t[which(t[[2]]=="all_SM"),3];
SS=t[which(t[[2]]=="all_SS"),3];

red_spectrum=c("#FF7777", "#FF6666", "#FF4747", "#FF3333", "#FF1717", "#FF0000");
green_spectrum=c("#77FF77", "#66FF66", "#47FF47", "#33FF33", "#17FF17", "#00FF00");
blue_spectrum=c("#7777FF", "#6666FF", "#4747FF", "#3333FF", "#1717FF", "#0000FF");

png(paste("plot_of_", category_name, "_areas_distributions.png", sep=""), height=8, width=9, units="in", res=600);

if(length(AA)>1000)
{
plot(x=AA, y=SS/AA, ylim=c(0,1), xlim=c(0, quantile(AA, p=(0:100)/100, names=FALSE)[99]), xlab="AA", ylab="Fraction", main=paste("Plot of ", category_name, " areas distributions", sep=""), col=densCols(AA, SS/AA, colramp=colorRampPalette(blue_spectrum)), pch=16, cex=0.5);
points(x=AA, y=MM/AA, col=densCols(AA, MM/AA, colramp=colorRampPalette(green_spectrum)), pch=16, cex=0.5);
points(x=AA, y=SM/AA, col=densCols(AA, SM/AA, colramp=colorRampPalette(red_spectrum)), pch=16, cex=0.5);
} else {
plot(x=AA, y=SS/AA, ylim=c(0,1), xlim=c(0, quantile(AA, p=(0:100)/100, names=FALSE)[99]), xlab="AA", ylab="Fraction", main=paste("Plot of ", category_name, " areas distributions", sep=""), col="blue", pch=16, cex=1.0);
points(x=AA, y=MM/AA, col="green", pch=16, cex=1.0);
points(x=AA, y=SM/AA, col="red", pch=16, cex=1.0);
}


legend("topleft", c("SS/AA", "MM/AA", "SM/AA"), pch=c(16, 16, 16), col=c("blue", "green", "red"));

dev.off();
