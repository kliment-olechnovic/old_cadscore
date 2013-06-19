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

png(paste("plot_of_", category_name, "_areas_distributions.png", sep=""), height=8, width=9, units="in", res=600);

plot(x=AA, y=SS/AA, ylim=c(0,1), xlim=c(0, quantile(AA, p=(0:100)/100, names=FALSE)[99]), xlab="AA", ylab="Fraction", main=paste("Plot of ", category_name, " areas distributions", sep=""), col="blue", pch=0);
points(x=AA, y=MM/AA, col="green", pch=1);
points(x=AA, y=SM/AA, col="red", pch=2);
legend("topleft", c("SS/AA", "MM/AA", "SM/AA"), pch=c(0, 1, 2), col=c("blue", "green", "red"));

dev.off();
