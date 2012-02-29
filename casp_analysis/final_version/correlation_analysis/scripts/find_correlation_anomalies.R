t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$domain>0),];

output_directory="correlation_anomalies";
dir.create(output_directory);

###################

x=c(0.425, 1.0);
y=c(0.0, 0.4);

a=(y[1]-y[2])/(x[1]-x[2]);
b=y[1]-a*x[1];

st=t[which((t$SS-(t$CASP_GDT_TS*a+b))<0.0),]

write.table(st, paste(output_directory, "/correlation_anomalies_for_low_SS_high_GDT_TS", sep=""), quote=FALSE, row.names=FALSE);

png(paste(output_directory, "/correlation_anomalies_for_low_SS_high_GDT_TS.png", sep=""), height=7, width=7, units="in", res=200);
plot(x=t$CASP_GDT_TS, y=t$SS, xlim=c(0, 1), ylim=c(0, 1), col="black", pch=16, cex=0.5, xlab="GDT_TS", ylab="SS");
points(x=st$CASP_GDT_TS, y=st$SS, col="red", pch=16, cex=0.5);
points(x=c(-1, 2), y=c(-1, 2)*a+b, col="blue", type="l");
dev.off();

###################

x=c(0.2, 0.4);
y=c(0.5, 0.6);

a=(y[1]-y[2])/(x[1]-x[2]);
b=y[1]-a*x[1];

st=t[which((t$AA-(t$CASP_GDT_TS*a+b))>0.0),]

write.table(st, paste(output_directory, "/correlation_anomalies_for_high_AA_low_GDT_TS", sep=""), quote=FALSE, row.names=FALSE);

png(paste(output_directory, "/correlation_anomalies_for_high_AA_low_GDT_TS.png", sep=""), height=7, width=7, units="in", res=200);
plot(x=t$CASP_GDT_TS, y=t$AA, xlim=c(0, 1), ylim=c(0, 1), col="black", pch=16, cex=0.5, xlab="GDT_TS", ylab="AA");
points(x=st$CASP_GDT_TS, y=st$AA, col="red", pch=16, cex=0.5);
points(x=c(-1, 2), y=c(-1, 2)*a+b, col="blue", type="l");
dev.off();
