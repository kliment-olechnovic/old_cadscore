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

max_contact_area=300;
t_any=t_any[which(t_any$contact_area<=max_contact_area),];

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

hist_bins=50;
hist_breaks=(0:hist_bins)*(max_contact_area/hist_bins);
hist_x=hist_breaks[1:(length(hist_breaks)-1)];

spec_cols=c(rgb(1, 0, 0, 0.5), rgb(0, 1, 0, 0.6), rgb(0, 0, 1, 0.7));

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

barplot(rbind(h_bb$counts, h_bB$counts, h_BB$counts), names.arg=h_bb$mids, col=spec_cols, border=NA, xlab="Area", ylab="Frequency", main="Subdivided histogram of stacking areas");
legend(0, 2500, c("pyrimidine-pyrimidine", "pyrimidine-purine", "purine-purine"), pch=c(15, 15, 15), col=spec_cols);

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

barplot(rbind(roman_h$counts, nonroman_h$counts), names.arg=roman_h$mids, col=spec_cols[1:2], border=NA, xlab="Area", ylab="Frequency", main="Subdivided histogram of siding areas");
legend(25, 2500, c("Pairings with two or three H-bonds", "Other pairings"), pch=c(15, 15, 15), col=spec_cols[1:2]);
