t1=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t2=read.table("mean_radius_enhanced_table", header=TRUE, stringsAsFactors=FALSE);

output_directory="simple_comparison_plots";
dir.create(output_directory);

diff_aa=t1$AA-t2$AA;
diff_ss=t1$SS-t2$SS;
diff_mm=t1$MM-t2$MM;

png(paste(output_directory, "/raw_differences.png", sep=""), height=5, width=5, units="in", res=200);
plot(x=diff_aa, y=diff_ss, xlim=c(-0.06, 0.06), ylim=c(-0.06, 0.06), col="red");
#points(x=diff_aa, y=diff_mm, col="green");
points(x=c(-1,1), y=c(-1,1), type="l");
points(x=c(-1,1), y=c(0,0), type="l");
points(x=c(0,0), y=c(-1,1), type="l");
dev.off();

#####

png(paste(output_directory, "/aa_minus_ss.png", sep=""), height=5, width=7, units="in", res=200);
hist(abs(t1$AA-t1$SS)-abs(t2$AA-t2$SS), breaks=100, xlim=c(-0.07, 0.07), xlab="spec_radius(AA-SS)-mean_radius(AA-SS)", main="");
points(x=c(0,0), y=c(0, 1000), type="l");
dev.off();

png(paste(output_directory, "/aa_minus_mm.png", sep=""), height=5, width=7, units="in", res=200);
hist(abs(t1$AA-t1$MM)-abs(t2$AA-t2$MM), breaks=100, xlim=c(-0.07, 0.07), xlab="spec_radius(AA-MM)-mean_radius(AA-MM)", main="");
points(x=c(0,0), y=c(0, 1000), type="l");
dev.off();

png(paste(output_directory, "/mm_minus_ss.png", sep=""), height=5, width=7, units="in", res=200);
hist(abs(t1$MM-t1$SS)-abs(t2$MM-t2$SS), breaks=100, xlim=c(-0.07, 0.07), xlab="spec_radius(MM-SS)-mean_radius(MM-SS)", main="");
points(x=c(0,0), y=c(0, 1000), type="l");
dev.off();

#####

png(paste(output_directory, "/aa.png", sep=""), height=5, width=7, units="in", res=200);
hist(t1$AA-t2$AA, breaks=100, xlim=c(-0.07, 0.07), xlab="spec_radius(AA)-mean_radius(AA)", main="");
points(x=c(0,0), y=c(0, 1000), type="l");
dev.off();

png(paste(output_directory, "/ss.png", sep=""), height=5, width=7, units="in", res=200);
hist(t1$SS-t2$SS, breaks=100, xlim=c(-0.07, 0.07), xlab="spec_radius(SS)-mean_radius(SS)", main="");
points(x=c(0,0), y=c(0, 1000), type="l");
dev.off();

png(paste(output_directory, "/mm.png", sep=""), height=5, width=7, units="in", res=200);
hist(t1$MM-t2$MM, breaks=100, xlim=c(-0.07, 0.07), xlab="spec_radius(MM)-mean_radius(MM)", main="");
points(x=c(0,0), y=c(0, 1000), type="l");
dev.off();


