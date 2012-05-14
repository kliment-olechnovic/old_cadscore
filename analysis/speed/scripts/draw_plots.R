t=read.table("summary_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$voroprot2_atoms>200),];
t=t[which(abs(t$voroprot2_atoms-t$qtfier_atoms)<30),];

output_directory="plots";
dir.create(output_directory);

ps=c(95, 100);
qs=quantile(t$voroprot2_atoms, probs=ps/100, names=FALSE);

options(scipen=5);

for(i in 1:length(ps))
{
	pv=ps[i];
	qv=qs[i];
	sel=which(t$voroprot2_atoms<=qv);
	png(paste(output_directory, paste("/running_times_", length(sel), ".png", sep=""), sep=""), height=5, width=7, units="in", res=600);
	plot(x=t$qtfier_atoms[sel], y=t$qtfier_time[sel], type="p", lwd=1.5, pch=0, col="red", main="", xlab="Atoms", ylab="Seconds");
	points(x=t$voroprot2_atoms[sel], y=t$voroprot2_time[sel], type="p", lwd=1.5, pch=1, col="blue");
	legend(0, max(t$qtfier_time[sel]), c("Our algorithm", "QTFier"), pch=c(1,0), col=c("blue", "red"));
	dev.off();
}

##########################

t=read.table("summary_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$voroprot2_atoms>200),];
t=t[which(t$voroprot2_atoms==t$qtfier_atoms),];

vds=(t$voroprot2_vertices-t$qtfier_vertices);
vdsr=vds/t$voroprot2_vertices;

png(paste(output_directory, "/vertices_differences_relative.png", sep=""), height=5, width=7, units="in", res=600);
plot(x=t$voroprot2_vertices, y=vdsr, type="p", lwd=1.5, pch=19, cex=0.5, col=densCols(t$voroprot2_vertices, vdsr), main="", xlab="Voronoi vertices", ylab="Relative difference");
dev.off();

png(paste(output_directory, "/vertices_differences.png", sep=""), height=5, width=7, units="in", res=600);
plot(x=t$voroprot2_vertices, log="x", y=vds, type="p", lwd=1.5, pch=19, cex=0.5, col=densCols(t$voroprot2_vertices, vdsr), main="", xlab="Voronoi vertices (logarithmic scale)", ylab="Difference");
dev.off();
