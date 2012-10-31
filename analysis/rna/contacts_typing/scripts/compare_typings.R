t_a=read.table("typed_contacts_ids_and_areas", header=FALSE, stringsAsFactors=FALSE);
t_b=read.table("mc_annotate_interactions_ids", header=FALSE, stringsAsFactors=FALSE);

stacking_t_a=t_a[which(t_a[[2]]=="stack"), ];

siding_t_a=t_a[which(t_a[[2]]=="side"), ];
siding_t_a=siding_t_a[which(siding_t_a[[3]]<200), ];

stacking_ids_b=t_b[which(t_b[[2]]=="stack"), 1];
stacking_ids_b=union(stacking_ids_b, stacking_ids_b);

siding_ids_b=t_b[which(t_b[[2]]=="side"), 1];
siding_ids_b=union(siding_ids_b, siding_ids_b);

length(stacking_t_a[[1]]);
length(stacking_ids_b);
length(intersect(stacking_t_a[[1]], stacking_ids_b));

length(siding_t_a[[1]]);
length(siding_ids_b);
length(intersect(siding_t_a[[1]], siding_ids_b));

missed_mc_annotate_stackings=t_b[which(is.element(t_b[[1]], setdiff(stacking_ids_b, intersect(stacking_t_a[[1]], stacking_ids_b)))),];
write.table(missed_mc_annotate_stackings, "missed_mc_annotate_stacking_interactions", quote=FALSE, row.names=FALSE, col.names=FALSE);

missed_mc_annotate_sidings=t_b[which(is.element(t_b[[1]], setdiff(siding_ids_b, intersect(siding_t_a[[1]], siding_ids_b)))),];
write.table(missed_mc_annotate_sidings, "missed_mc_annotate_siding_interactions", quote=FALSE, row.names=FALSE, col.names=FALSE);

png("histogram_of_all_areas.png", height=4.8, width=4.4, units="in", res=300);
hist(t_a[[3]], breaks=100, xlab="Area", main="Histogram of all areas");
hist(t_a[which(is.element(t_a[[1]], union(stacking_ids_b, siding_ids_b))), 3], breaks=100, add=TRUE, col="red");
dev.off();

png("histogram_of_stacking_areas.png", height=4.8, width=4.4, units="in", res=300);
hist(stacking_t_a[[3]], breaks=100, xlab="Area", main="Histogram of stacking areas");
hist(stacking_t_a[which(is.element(stacking_t_a[[1]], stacking_ids_b)), 3], breaks=100, add=TRUE, col="red");
dev.off();

png("histogram_of_siding_areas.png", height=4.8, width=4.4, units="in", res=300);
hist(siding_t_a[[3]], breaks=100, xlab="Area", main="Histogram of siding areas");
hist(siding_t_a[which(is.element(siding_t_a[[1]], siding_ids_b)), 3], breaks=100, add=TRUE, col="red");
dev.off();

png("plot_of_all_areas_contribution.png", height=4.8, width=4.4, units="in", res=300);
v=t_a[which(is.element(t_a[[1]], union(stacking_ids_b, siding_ids_b))), 3];
ds=density(v);
sel=which(ds$x>0);
plot(x=ds$x[sel], y=ds$y[sel]*ds$x[sel]*length(v), xlab="Area", ylab="Area * Frequency", main="Plot of all areas contribution", type="l", col="red", lwd=5.0);
v=t_a[[3]];
ds=density(v);
sel=which(ds$x>0);
points(x=ds$x[sel], y=ds$y[sel]*ds$x[sel]*length(v), type="l");
dev.off();

png("plot_of_stacking_areas_contribution.png", height=4.8, width=4.4, units="in", res=300);
v=stacking_t_a[which(is.element(stacking_t_a[[1]], stacking_ids_b)), 3];
ds=density(v);
sel=which(ds$x>0);
plot(x=ds$x[sel], y=ds$y[sel]*ds$x[sel]*length(v), xlab="Area", ylab="Area * Frequency", main="Plot of stacking areas contribution", type="l", col="red", lwd=5.0);
v=stacking_t_a[[3]];
ds=density(v);
sel=which(ds$x>0);
points(x=ds$x[sel], y=ds$y[sel]*ds$x[sel]*length(v), type="l");
dev.off();

png("plot_of_siding_areas_contribution.png", height=4.8, width=4.4, units="in", res=300);
v=siding_t_a[which(is.element(siding_t_a[[1]], siding_ids_b)), 3];
ds=density(v);
sel=which(ds$x>0);
plot(x=ds$x[sel], y=ds$y[sel]*ds$x[sel]*length(v), xlab="Area", ylab="Area * Frequency", main="Plot of siding areas contribution", type="l", col="red", lwd=5.0);
v=siding_t_a[[3]];
ds=density(v);
sel=which(ds$x>0);
points(x=ds$x[sel], y=ds$y[sel]*ds$x[sel]*length(v), type="l");
dev.off();
