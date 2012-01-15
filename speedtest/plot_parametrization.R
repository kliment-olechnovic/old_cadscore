lct=read.table("parametrization/lowcounts.list", header=FALSE, stringsAsFactors=FALSE);
lc=lct[[2]][which(lct[[1]]=="lowcount")];
time=lct[[2]][which(lct[[1]]=="real")];
png("parametrization/lowcount_vs_time.png", width=7, height=7, units="in", res=300);
plot(x=lc, y=time, col="black", type="l", main="LowCount vs Time", xlab="LowCount", ylab="Time");
dev.off();

rt=read.table("parametrization/radii.list", header=FALSE, stringsAsFactors=FALSE);
r=rt[[2]][which(rt[[1]]=="radius")];
time=rt[[2]][which(rt[[1]]=="real")];
png("parametrization/radius_vs_time.png", width=7, height=7, units="in", res=300);
plot(x=r, y=time, col="black", type="l", main="Radius vs Time", xlab="Radius", ylab="Time");
dev.off();