t_contacts=read.table("../reformatting_annotations/output/stack_and_side_by_voroprot2_contacts", header=TRUE, stringsAsFactors=FALSE);
t_stack_mcannotate=read.table("../reformatting_annotations/output/stack_by_mc_annotate", header=TRUE, stringsAsFactors=FALSE);
t_side_mcannotate=read.table("../reformatting_annotations/output/side_by_mc_annotate", header=TRUE, stringsAsFactors=FALSE);

t_contacts=t_contacts[which(t_contacts$pdb_id!="1QCU"),];
t_stack_mcannotate=t_stack_mcannotate[which(t_stack_mcannotate$pdb_id!="1QCU"),];
t_side_mcannotate=t_side_mcannotate[which(t_side_mcannotate$pdb_id!="1QCU"),];

ids_stack_mcannotate=union(t_stack_mcannotate$contact_id, t_stack_mcannotate$contact_id);
ids_side_mcannotate=union(t_side_mcannotate$contact_id, t_side_mcannotate$contact_id);
ids_mcannotate=union(ids_stack_mcannotate, ids_side_mcannotate);
pdb_ids_mcannotate=union(t_stack_mcannotate$pdb_id, t_side_mcannotate$pdb_id);

#################

t_any=t_contacts;
t_any=t_any[which(is.element(t_any$pdb_id, pdb_ids_mcannotate)),];

max_contact_area=300;
t_any=t_any[which(t_any$contact_area<=max_contact_area),];

allowed_basenames=c("A", "U", "G", "C", "I", "DA", "DG", "DC");
t_any=t_any[which(is.element(t_any$first_base_name, allowed_basenames)),];
t_any=t_any[which(is.element(t_any$second_base_name, allowed_basenames)),];

t_any_orig=t_any;

t_any$first_base_name[which(is.element(t_any$first_base_name, c("A", "G", "I", "DA", "DG")))]="B";
t_any$first_base_name[which(is.element(t_any$first_base_name, c("U", "C", "DC")))]="b";
t_any$second_base_name[which(is.element(t_any$second_base_name, c("A", "G", "I", "DA", "DG")))]="B";
t_any$second_base_name[which(is.element(t_any$second_base_name, c("U", "C", "DC")))]="b";

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

t_any_orig_not_sel=t_any_orig[which(!is.element(t_any_orig$contact_id, ids_mcannotate)),];
t_any_orig_not_sel=t_any_orig_not_sel[rev(order(t_any_orig_not_sel$contact_area)),];
write.table(t_any_orig_not_sel, "./output/differences_from_mc_annotate", quote=FALSE, row.names=FALSE, col.names=TRUE);

#################

length(t_stack_mcannotate[[1]]);
missed_stack=t_stack_mcannotate[which(is.element(t_stack_mcannotate$contact_id, setdiff(ids_stack_mcannotate, t_any$contact_id))),];
length(missed_stack[[1]]);
write.table(missed_stack, "./output/missed_mc_annotate_stacking_interactions", quote=FALSE, row.names=FALSE, col.names=TRUE);

length(t_side_mcannotate[[1]]);
missed_side=t_side_mcannotate[which(is.element(t_side_mcannotate$contact_id, setdiff(ids_side_mcannotate, t_any$contact_id))),];
length(missed_side[[1]]);
write.table(missed_side, "./output/missed_mc_annotate_siding_interactions", quote=FALSE, row.names=FALSE, col.names=TRUE);

#################

hist_bins=50;
hist_bin_size=(max_contact_area/hist_bins);
hist_breaks=(0:hist_bins)*hist_bin_size;
hist_x=hist_breaks[1:(length(hist_breaks)-1)];

spec_cols=c(rgb(1, 0, 0, 0.5), rgb(0, 1, 0, 0.6), rgb(0, 0, 1, 0.7), rgb(1, 0, 1, 0.8), rgb(0, 1, 1, 0.9));

#################

hist_any=hist(t_any$contact_area, breaks=hist_breaks, xlim=c(0, max_contact_area), xlab="Area", main="Histogram of all areas");
hist_any_sel=hist(t_any_sel$contact_area, breaks=hist_breaks, add=TRUE, col=spec_cols[1]);

plot(hist_x, hist_any$counts*hist_x, xlim=c(0, max_contact_area), type="l", col="black", xlab="Area", ylab="Frequency*Area", main="Contributions of all areas");
points(hist_x, hist_any_sel$counts*hist_x, type="l", col=spec_cols[1], lwd=5);

#####

hist_stack=hist(t_stack$contact_area, breaks=hist_breaks, xlim=c(0, max_contact_area), xlab="Area", main="Histogram of stacking areas");
hist_stack_sel=hist(t_stack_sel$contact_area, breaks=hist_breaks, add=TRUE, col=spec_cols[1]);

plot(hist_x, hist_stack$counts*hist_x, xlim=c(0, max_contact_area), type="l", col="black", xlab="Area", ylab="Frequency*Area", main="Contributions of stacking areas");
points(hist_x, hist_stack_sel$counts*hist_x, type="l", col=spec_cols[1], lwd=5);

#####

hist_side=hist(t_side$contact_area, breaks=hist_breaks, xlim=c(0, max_contact_area), xlab="Area", main="Histogram of siding areas");
hist_side_sel=hist(t_side_sel$contact_area, breaks=hist_breaks, add=TRUE, col=spec_cols[1]);

plot(hist_x, hist_side$counts*hist_x, xlim=c(0, max_contact_area), type="l", col="black", xlab="Area", ylab="Frequency*Area", main="Contributions of siding areas");
points(hist_x, hist_side_sel$counts*hist_x, type="l", col=spec_cols[1], lwd=5);

#################

h_bb=hist(t_stack_sel[which(t_stack_sel$basepairs=="bb"), "contact_area"], breaks=hist_breaks, plot=FALSE);
h_bB=hist(t_stack_sel[which(t_stack_sel$basepairs=="bB"), "contact_area"], breaks=hist_breaks, plot=FALSE);
h_BB=hist(t_stack_sel[which(t_stack_sel$basepairs=="BB"), "contact_area"], breaks=hist_breaks, plot=FALSE);

barplot(rbind(h_bb$counts, h_bB$counts, h_BB$counts), names.arg=h_bb$mids, col=spec_cols[1:3], border=NA, xlab="Area", ylab="Frequency", main="Subdivided histogram of stacking areas");
legend(0, 2500, c("pyrimidine-pyrimidine", "pyrimidine-purine", "purine-purine"), pch=c(15, 15, 15), col=spec_cols[1:3]);

#################

roman_codes=c("P_I", "P_II", "P_III", "P_IV", "P_V", "P_VI", "P_VII", "P_VIII", "P_IX", "P_X", "P_XI", "P_XII", "P_XIII", "P_XIV", "P_XV", "P_XVI", "P_XVII", "P_XVIII", "P_XIX", "P_XX", "P_XXI", "P_XXII", "P_XXIII", "P_XXIV", "P_XXV", "P_XXVI", "P_XXVII", "P_XXVIII");

roman_sel=which(is.element(t_side_mcannotate$prop5, roman_codes));
roman_sel=union(roman_sel, which(is.element(t_side_mcannotate$prop6, roman_codes)));
roman_sel=union(roman_sel, which(is.element(t_side_mcannotate$prop7, roman_codes)));
roman_sel=union(roman_sel, which(is.element(t_side_mcannotate$prop8, roman_codes)));
roman_sel=union(roman_sel, which(is.element(t_side_mcannotate$prop9, roman_codes)));
roman_sel=union(roman_sel, which(is.element(t_side_mcannotate$prop10, roman_codes)));
nonroman_sel=setdiff(1:length(t_side_mcannotate[[1]]), roman_sel);

roman_ids_side_mcannotate=t_side_mcannotate$contact_id[roman_sel];
roman_ids_side_mcannotate=union(roman_ids_side_mcannotate, roman_ids_side_mcannotate);
roman_t_side_sel=t_side[which(is.element(t_side$contact_id, roman_ids_side_mcannotate)),];

nonroman_ids_side_mcannotate=t_side_mcannotate$contact_id[nonroman_sel];
nonroman_ids_side_mcannotate=union(nonroman_ids_side_mcannotate, nonroman_ids_side_mcannotate);
nonroman_t_side_sel=t_side[which(is.element(t_side$contact_id, nonroman_ids_side_mcannotate)),];

roman_h=hist(roman_t_side_sel$contact_area, breaks=hist_breaks, plot=FALSE);
nonroman_h=hist(nonroman_t_side_sel$contact_area, breaks=hist_breaks, plot=FALSE);

barplot(rbind(roman_h$counts, nonroman_h$counts), names.arg=roman_h$mids, col=spec_cols[4:5], border=NA, xlab="Area", ylab="Frequency", main="Subdivided histogram of siding areas");
legend(25, 2500, c("Pairings with two or three H-bonds", "Other pairings"), pch=c(15, 15), col=spec_cols[4:5]);

#################

max_ylim=max(c(h_BB$counts, h_bB$counts, h_bb$counts, roman_h$counts, nonroman_h$counts));
barplot(rbind(h_bb$counts, h_bB$counts, h_BB$counts, roman_h$counts, nonroman_h$counts), names.arg=h_bb$mids, col=spec_cols[1:5], border=NA, ylim=c(0, max_ylim*1.5), xlab="Area", ylab="Frequency", main="Subdivided histogram of all areas");
legend("topleft", c("Stacking pyrimidine-pyrimidine", "Stacking pyrimidine-purine", "Stacking purine-purine", "Pairings with two or three H-bonds", "Other pairings"), pch=c(15, 15, 15, 15, 15), col=spec_cols[1:5]);

max_ylim=max(c(h_BB$counts, h_bB$counts, h_bb$counts, roman_h$counts, nonroman_h$counts));
png("./area_histograms.png", height=10, width=4, units="in", res=300);
old_par=par(mfrow=c(5, 1));
par(mar=c(4.0, 4.0, 1.5, 1.0));
barplot(h_BB$counts, col=spec_cols[3], ylim=c(0, max_ylim), names.arg=h_bb$mids, xlab=NULL, ylab="Frequency", main=NULL);
title("Stacking purine-purine", adj=1)
barplot(h_bB$counts, col=spec_cols[2], ylim=c(0, max_ylim), names.arg=h_bb$mids, xlab=NULL, ylab="Frequency", main=NULL);
title("Stacking pyrimidine-purine", adj=1)
barplot(h_bb$counts, col=spec_cols[1], ylim=c(0, max_ylim), names.arg=h_bb$mids, xlab=NULL, ylab="Frequency", main=NULL);
title("Stacking pyrimidine-pyrimidine", adj=1)
barplot(roman_h$counts, col=spec_cols[4], ylim=c(0, max_ylim), names.arg=h_bb$mids, xlab=NULL, ylab="Frequency", main=NULL);
title("Pairings with two or three H-bonds", adj=1)
barplot(nonroman_h$counts, col=spec_cols[5], ylim=c(0, max_ylim), names.arg=h_bb$mids, xlab="Area", ylab="Frequency", main=NULL);
title("Other pairings", adj=1)
par(old_par);
dev.off();

max_ylim=max(c(h_BB$counts*hist_x, h_bB$counts*hist_x, h_bb$counts*hist_x, roman_h$counts*hist_x, nonroman_h$counts*hist_x));
plot(hist_x, h_BB$counts*hist_x, type="l", col=spec_cols[3], lwd=2, xlim=c(0, max_contact_area), ylim=c(0, max_ylim), xlab="Area", ylab="Frequency * Area", main="Contributions of all areas");
points(hist_x, h_bB$counts*hist_x, type="l", col=spec_cols[2], lwd=2);
points(hist_x, h_bb$counts*hist_x, type="l", col=spec_cols[1], lwd=2);
points(hist_x, roman_h$counts*hist_x, type="l", col=spec_cols[4], lwd=2);
points(hist_x, nonroman_h$counts*hist_x, type="l", col=spec_cols[5], lwd=2);
legend("topleft", c("Stacking pyrimidine-pyrimidine", "Stacking pyrimidine-purine", "Stacking purine-purine", "Pairings with two or three H-bonds", "Other pairings"), pch=c(15, 15, 15, 15, 15), col=spec_cols[1:5]);

#################

t_stack_sel_sorted=t_stack_sel[order(t_stack_sel$contact_area), c("contact_id", "contact_area")];
write.table(t_stack_sel_sorted, "./output/sorted_stack_contacts", quote=FALSE, row.names=FALSE, col.names=TRUE);

t_side_sel_sorted=t_side_sel[order(t_side_sel$contact_area), c("contact_id", "contact_area")];
write.table(t_side_sel_sorted, "./output/sorted_side_contacts", quote=FALSE, row.names=FALSE, col.names=TRUE);

#################

histograms_summary=data.frame(area=hist_x, f_all=hist_any$counts, f_all_confirmed=hist_any_sel$counts, f_stack=hist_stack$counts, f_stack_confirmed=hist_stack_sel$counts, f_side=hist_side$counts, f_side_confirmed=hist_side_sel$counts, f_stack_confirmed_pp=h_bb$counts, f_stack_confirmed_pP=h_bB$counts, f_stack_confirmed_PP=h_BB$counts, f_side_confirmed_strong=roman_h$counts, f_side_confirmed_weak=nonroman_h$counts);
write.table(histograms_summary, "./output/histograms_summary.txt", quote=FALSE, row.names=FALSE, col.names=TRUE);
