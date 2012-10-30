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

png("histogram_of_stacking_areas.png", height=4.8, width=4.4, units="in", res=300);
hist(stacking_t_a[[3]], breaks=100, xlab="Area", main="Histogram of stacking areas");
hist(stacking_t_a[which(is.element(stacking_t_a[[1]], stacking_ids_b)), 3], breaks=100, add=TRUE, col="red");
dev.off();

png("histogram_of_siding_areas.png", height=4.8, width=4.4, units="in", res=300);
hist(siding_t_a[[3]], breaks=100, xlab="Area", main="Histogram of siding areas");
hist(siding_t_a[which(is.element(siding_t_a[[1]], siding_ids_b)), 3], breaks=100, add=TRUE, col="red");
dev.off();

png("plot_of_stacking_areas_contribution.png", height=4.8, width=4.4, units="in", res=300);
ds=density(stacking_t_a[[3]]);
sel=which(ds$x>0);
plot(x=ds$x[sel], y=ds$y[sel]*ds$x[sel], xlab="Area", ylab="Area * Frequency", main="Plot of stacking areas contribution");
dev.off();

png("plot_of_siding_areas_contribution.png", height=4.8, width=4.4, units="in", res=300);
ds=density(siding_t_a[[3]]);
sel=which(ds$x>0);
plot(x=ds$x[sel], y=ds$y[sel]*ds$x[sel], xlab="Area", ylab="Area * Frequency", main="Plot of siding areas contribution");
dev.off();
