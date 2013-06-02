t_contacts=read.table("../reformatting_annotations/output/stack_and_side_by_voroprot2_contacts", header=TRUE, stringsAsFactors=FALSE);
t_faces=read.table("../reformatting_annotations/output/stack_and_side_by_voroprot2_faces", header=TRUE, stringsAsFactors=FALSE);
t_stack_mcannotate=read.table("../reformatting_annotations/output/stack_by_mc_annotate", header=TRUE, stringsAsFactors=FALSE);
t_side_mcannotate=read.table("../reformatting_annotations/output/side_by_mc_annotate", header=TRUE, stringsAsFactors=FALSE);

ids_stack_mcannotate=union(t_stack_mcannotate$contact_id, t_stack_mcannotate$contact_id);
ids_side_mcannotate=union(t_side_mcannotate$contact_id, t_side_mcannotate$contact_id);
ids_mcannotate=union(ids_stack_mcannotate, ids_side_mcannotate);
pdb_ids_mcannotate=union(t_stack_mcannotate$pdb_id, t_side_mcannotate$pdb_id);

#################

t_any=t_contacts;
t_any=t_any[which(is.element(t_any$pdb_id, pdb_ids_mcannotate)),];

allowed_basenames=c("A", "U", "G", "C");
t_any=t_any[which(is.element(t_any$first_base_name, allowed_basenames)),];
t_any=t_any[which(is.element(t_any$second_base_name, allowed_basenames)),];

t_any$first_base_name[which(is.element(t_any$first_base_name, c("A", "G")))]="B";
t_any$first_base_name[which(is.element(t_any$first_base_name, c("U", "C")))]="b";
t_any$second_base_name[which(is.element(t_any$second_base_name, c("A", "G")))]="B";
t_any$second_base_name[which(is.element(t_any$second_base_name, c("U", "C")))]="b";

basepairs=rep("xx", length(t_any[[1]]));
for(i in 1:length(basepairs))
{
  if(t_any$first_base_name[i]<t_any$second_base_name[i])
  {
    basepairs[i]=paste(t_any$first_base_name[i], t_any$second_base_name[i], sep="");
  }
  else
  {
    basepairs[i]=paste(t_any$second_base_name[i], t_any$first_base_name[i], sep="");
  }
}
t_any$basepairs=basepairs;
t_any$basepairs_factors=factor(t_any$basepairs);

t_stack=t_any[which(t_any$contact_type=="stack"),];
t_side=t_any[which(t_any$contact_type=="side"),];

t_any_sel=t_any[which(is.element(t_any$contact_id, ids_mcannotate)),];
t_stack_sel=t_stack[which(is.element(t_stack$contact_id, ids_stack_mcannotate)),];
t_side_sel=t_side[which(is.element(t_side$contact_id, ids_side_mcannotate)),];

#################

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

#################

plot(density(t_stack_sel[, "contact_area"]), col="black", xlab="Area", main="Densities for stacking areas of three classes", xlim=c(0, 250), ylim=c(0, 0.035));
points(density(t_stack_sel[which(t_stack_sel$basepairs=="bb"), "contact_area"]), col="red", type="l");
points(density(t_stack_sel[which(t_stack_sel$basepairs=="bB"), "contact_area"]), col="green", type="l");
points(density(t_stack_sel[which(t_stack_sel$basepairs=="BB"), "contact_area"]), col="blue", type="l");

h_breaks=0:20*(300/20);
h_xx=hist(t_stack_sel[, "contact_area"], breaks=h_breaks);
h_bb=hist(t_stack_sel[which(t_stack_sel$basepairs=="bb"), "contact_area"], breaks=h_breaks);
h_bB=hist(t_stack_sel[which(t_stack_sel$basepairs=="bB"), "contact_area"], breaks=h_breaks);
h_BB=hist(t_stack_sel[which(t_stack_sel$basepairs=="BB"), "contact_area"], breaks=h_breaks);

plot(h_xx$mids, h_xx$counts, type="h", lwd="5");
points(h_BB$mids, h_BB$counts+h_bB$counts+h_bb$counts, type="h", lwd="5", col="blue");
points(h_bB$mids, h_bB$counts+h_bb$counts, type="h", lwd="5", col="green");
points(h_bb$mids, h_bb$counts, type="h", lwd="5", col="red");

plot(h_xx$mids, h_xx$counts, type="h", lwd="5");
points(h_BB$mids+2, h_BB$counts, type="h", lwd="5", col="blue");
points(h_bB$mids+4, h_bB$counts, type="h", lwd="5", col="green");
points(h_bb$mids+6, h_bb$counts, type="h", lwd="5", col="red");
