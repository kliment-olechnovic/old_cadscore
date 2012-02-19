t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$domain>0),];

sel1=which(t$AA>0.5);
sel2=sel1[which(t$CASP_GDT_TS[sel1]<0.4)];

t=t[sel2,];

write.table(t, "correlation_anomalies", quote=FALSE, row.names=FALSE);
