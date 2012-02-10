ca=commandArgs(TRUE);
t1=read.table(ca[1], header=TRUE);
t2=read.table(ca[2], header=TRUE);
t=merge(t1, t2);
write.table(t, ca[3], quote=FALSE, row.names=FALSE);
