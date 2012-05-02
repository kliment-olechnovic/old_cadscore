t=read.table("summary_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$voroprot2_atoms==t$qtfier_atoms),];

output_directory="plots";
dir.create(output_directory);

png(paste(output_directory, "/running_times.png", sep=""), height=5, width=7, units="in", res=600);

qtfier_sel=order(t$qtfier_atoms);
plot(x=t$qtfier_atoms[qtfier_sel], y=t$qtfier_time[qtfier_sel], type="p", lwd=1.5, pch=0, col="red", main="Running times", xlab="Atoms", ylab="Seconds");

voroprot2_sel=order(t$voroprot2_atoms);
points(x=t$voroprot2_atoms[voroprot2_sel], y=t$voroprot2_time[voroprot2_sel], type="p", lwd=1.5, pch=1, col="blue");

legend(0, 200, c("Our algorithm", "QTFier"), pch=c(1,0), col=c("blue", "red"));

dev.off();
