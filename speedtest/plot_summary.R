vq01=read.table("summary/vq.p0.s1.table", header=TRUE, stringsAsFactors=FALSE);
vq00=read.table("summary/vq.p0.s0.table", header=TRUE, stringsAsFactors=FALSE);
vq10=read.table("summary/vq.p1.s0.table", header=TRUE, stringsAsFactors=FALSE);
qtf=read.table("summary/qtf.table", header=TRUE, stringsAsFactors=FALSE);

vq01=vq01[order(vq01$atoms),];
vq00=vq00[order(vq00$atoms),];
vq10=vq10[order(vq10$atoms),];
qtf=qtf[order(qtf$atoms),];

####################

png("summary/atoms_vs_time.png", width=7, height=7, units="in", res=300);

plot(x=vq01$atoms, y=vq01$real, col="purple", type="l", main="Calculation time", xlab="Atoms", ylab="Time");
points(x=vq01$atoms, y=vq01$real, col="purple", type="p", pch=16);

points(x=vq00$atoms, y=vq00$real, col="blue", type="l");
points(x=vq00$atoms, y=vq00$real, col="blue", type="p", pch=16);

points(x=vq10$atoms, y=vq10$real, col="green", type="l");
points(x=vq10$atoms, y=vq10$real, col="green", type="p", pch=16);

points(x=qtf$atoms, y=qtf$real, col="red", type="l");
points(x=qtf$atoms, y=qtf$real, col="red", type="p", pch=16);

dev.off();

####################

png("summary/atoms_vs_vertices_diff.png", width=7, height=7, units="in", res=300);

plot(x=vq01$atoms, y=vq00$vertices-vq01$vertices, col="black", type="l", main="Voronoi vertices count difference", xlab="Atoms", ylab="Difference");
points(x=vq01$atoms, y=vq00$vertices-vq01$vertices, col="black", type="p", pch=16);

dev.off();

####################

png("summary/atoms_vs_vertices_diff_all.png", width=7, height=7, units="in", res=300);

plot(ylim=c(min(vq10$vertices-qtf$vertices), max(vq01$vertices-qtf$vertices)), x=vq01$atoms, y=vq01$vertices-qtf$vertices, col="purple", type="l", main="Voronoi vertices count difference", xlab="Atoms", ylab="Difference");
points(x=vq01$atoms, y=vq01$vertices-qtf$vertices, col="purple", type="p", pch=16);

points(x=vq00$atoms, y=vq00$vertices-qtf$vertices, col="blue", type="l");
points(x=vq00$atoms, y=vq00$vertices-qtf$vertices, col="blue", type="p", pch=16);

points(x=vq10$atoms, y=vq10$vertices-qtf$vertices, col="green", type="l");
points(x=vq10$atoms, y=vq10$vertices-qtf$vertices, col="green", type="p", pch=16);

points(x=qtf$atoms, y=qtf$vertices-qtf$vertices, col="red", type="l");
points(x=qtf$atoms, y=qtf$vertices-qtf$vertices, col="red", type="p", pch=16);

dev.off();