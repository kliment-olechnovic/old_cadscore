t_contacts=read.table("../reformatting_annotations/output/stack_and_side_by_voroprot2_contacts", header=TRUE, stringsAsFactors=FALSE);
t_faces=read.table("../reformatting_annotations/output/stack_and_side_by_voroprot2_faces", header=TRUE, stringsAsFactors=FALSE);
t_stack_mcannotate=read.table("../reformatting_annotations/output/stack_by_mc_annotate", header=TRUE, stringsAsFactors=FALSE);
t_side_mcannotate=read.table("../reformatting_annotations/output/side_by_mc_annotate", header=TRUE, stringsAsFactors=FALSE);

ids_stack_mcannotate=union(t_stack_mcannotate$contact_id, t_stack_mcannotate$contact_id);
ids_side_mcannotate=union(t_side_mcannotate$contact_id, t_side_mcannotate$contact_id);
ids_mcannotate=union(ids_stack_mcannotate, ids_side_mcannotate);

#################

t_any=t_contacts;
t_stack=t_any[which(t_any$contact_type=="stack"),];
t_side=t_any[which(t_any$contact_type=="side"),];

t_any_sel=t_any[which(is.element(t_any$contact_id, ids_mcannotate)),];
t_stack_sel=t_stack[which(is.element(t_stack$contact_id, ids_stack_mcannotate)),];
t_side_sel=t_side[which(is.element(t_side$contact_id, ids_side_mcannotate)),];

#####

hist(t_any$contact_area, breaks=100, xlab="Area", main="Histogram of all areas", xlim=c(0, 250));
hist(t_any_sel$contact_area, breaks=100, add=TRUE, col="red");

ds_any=density(t_any$contact_area);
ds_any_sel=density(t_any_sel$contact_area);

plot(ds_any$x, ds_any$y*length(t_any[[1]]), type="l", col="blue", xlim=c(0, 250), main="Densities for all areas", xlab="Area", ylab="Frequency");
points(ds_any_sel$x, ds_any_sel$y*length(t_any_sel[[1]]), type="l", col="red");

plot(ds_any$x, ds_any$y*length(t_any[[1]])*ds_any$x, type="l", col="blue", xlim=c(0, 250), main="Densities for all contributions of areas", xlab="Area", ylab="Area * Frequency");
points(ds_any_sel$x, ds_any_sel$y*length(t_any_sel[[1]])*ds_any_sel$x, type="l", col="red");

#####

hist(t_stack$contact_area, breaks=100, xlab="Area", main="Histogram of stacking areas", xlim=c(0, 250));
hist(t_stack_sel$contact_area, breaks=100, add=TRUE, col="red");

ds_stack=density(t_stack$contact_area);
ds_stack_sel=density(t_stack_sel$contact_area);

plot(ds_stack$x, ds_stack$y*length(t_stack[[1]]), type="l", col="blue", xlim=c(0, 250), main="Densities for stacking areas", xlab="Area", ylab="Frequency");
points(ds_stack_sel$x, ds_stack_sel$y*length(t_stack_sel[[1]]), type="l", col="red");

plot(ds_stack$x, ds_stack$y*length(t_stack[[1]])*ds_stack$x, type="l", col="blue", xlim=c(0, 250), main="Densities for contributions of stacking areas", xlab="Area", ylab="Area * Frequency");
points(ds_stack_sel$x, ds_stack_sel$y*length(t_stack_sel[[1]])*ds_stack_sel$x, type="l", col="red");

#####

hist(t_side$contact_area, breaks=100, xlab="Area", main="Histogram of siding areas", xlim=c(0, 250));
hist(t_side_sel$contact_area, breaks=100, add=TRUE, col="red");

ds_side=density(t_side$contact_area);
ds_side_sel=density(t_side_sel$contact_area);

plot(ds_side$x, ds_side$y*length(t_side[[1]]), type="l", col="blue", xlim=c(0, 250), main="Densities for siding areas", xlab="Area", ylab="Frequency");
points(ds_side_sel$x, ds_side_sel$y*length(t_side_sel[[1]]), type="l", col="red");

plot(ds_side$x, ds_side$y*length(t_side[[1]])*ds_side$x, type="l", col="blue", xlim=c(0, 250), main="Densities for contributions of siding areas", xlab="Area", ylab="Area * Frequency");
points(ds_side_sel$x, ds_side_sel$y*length(t_side_sel[[1]])*ds_side_sel$x, type="l", col="red");
