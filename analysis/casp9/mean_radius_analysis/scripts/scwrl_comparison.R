t1=read.table("scwrl_table_radii_spec", header=TRUE, stringsAsFactors=FALSE);
t2=read.table("scwrl_table_radii_avg", header=TRUE, stringsAsFactors=FALSE);

sel=which(t1$SS>0.1);
sel=intersect(sel, which(t2$SS>0.1));

t1=t1[sel,];
t2=t2[sel,];

output_directory="scwrl_comparison_plots";
dir.create(output_directory);

diff_aa=t1$AA-t2$AA;
diff_ss=t1$SS-t2$SS;
diff_mm=t1$MM-t2$MM;

png(paste(output_directory, "/raw_differences.png", sep=""), height=5, width=5, units="in", res=200);
plot(x=diff_aa, y=diff_ss, xlim=c(-0.06, 0.06), ylim=c(-0.06, 0.06), col="red", ylab="diff_ss and diff_mm", pch=1);
points(x=diff_aa, y=diff_mm, col="green", pch=0);
points(x=c(-1,1), y=c(-1,1), type="l");
points(x=c(-1,1), y=c(0,0), type="l");
points(x=c(0,0), y=c(-1,1), type="l");
legend(-0.06, 0.06, c("diff_ss", "diff_mm"), pch=c(1,0), col=c("red", "green"));
dev.off();

