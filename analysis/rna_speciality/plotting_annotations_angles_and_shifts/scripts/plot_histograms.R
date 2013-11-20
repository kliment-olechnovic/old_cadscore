library(plotrix);

t=read.table("./output/annotations_with_angles_and_shifts", header=TRUE, stringsAsFactors=FALSE);
dt=read.table("../comparing_annotations/output/differences_from_mc_annotate", header=TRUE, stringsAsFactors=FALSE);
st=t[which(is.element(t$contact_id, dt$contact_id)),];

t_stack=t[which(t$contact_type=="na_stacking"),];
t_side=t[which(t$contact_type=="na_siding"),];

st_stack=st[which(st$contact_type=="na_stacking"),];
st_side=st[which(st$contact_type=="na_siding"),];

########################

max_angle=90;
hist_bins=50;
hist_bin_size=(max_angle/hist_bins);
hist_breaks=(0:hist_bins)*hist_bin_size;
hist_x=hist_breaks[1:(length(hist_breaks)-1)];

hist(acos(abs(t$shift_cos))*180/pi, breaks=hist_breaks, xlim=c(0, max_angle), col=rgb(1, 1, 1, 1), xlab="Angle", main="Histograms of shift angles for all");
hist(acos(abs(t_stack$shift_cos))*180/pi, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
hist(acos(abs(t_side$shift_cos))*180/pi, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

weighted.hist(acos(abs(t$shift_cos))*180/pi, t$area, breaks=hist_breaks, xlim=c(0, max_angle), col=rgb(1, 1, 1, 1), xlab="Angle", main="Weighted histograms of shift angles for all");
weighted.hist(acos(abs(t_stack$shift_cos))*180/pi, t_stack$area, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
weighted.hist(acos(abs(t_side$shift_cos))*180/pi, t_side$area, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

hist(acos(abs(st$shift_cos))*180/pi, breaks=hist_breaks, xlim=c(0, max_angle), col=rgb(1, 1, 1, 1), xlab="Angle", main="Histograms of shift angles for differed");
hist(acos(abs(st_stack$shift_cos))*180/pi, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
hist(acos(abs(st_side$shift_cos))*180/pi, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

weighted.hist(acos(abs(st$shift_cos))*180/pi, st$area, breaks=hist_breaks, xlim=c(0, max_angle), col=rgb(1, 1, 1, 1), xlab="Angle", main="Weighted histograms of shift angles for differed");
weighted.hist(acos(abs(st_stack$shift_cos))*180/pi, st_stack$area, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
weighted.hist(acos(abs(st_side$shift_cos))*180/pi, st_side$area, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

hist(acos(abs(t$nncos))*180/pi, breaks=hist_breaks, xlim=c(0, max_angle), col=rgb(1, 1, 1, 1), xlab="Angle", main="Histograms of inter-normal angles for all");
hist(acos(abs(t_stack$nncos))*180/pi, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
hist(acos(abs(t_side$nncos))*180/pi, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

weighted.hist(acos(abs(t$nncos))*180/pi, t$area, breaks=hist_breaks, xlim=c(0, max_angle), col=rgb(1, 1, 1, 1), xlab="Angle", main="Weighted histograms of inter-normal angles for all");
weighted.hist(acos(abs(t_stack$nncos))*180/pi, t_stack$area, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
weighted.hist(acos(abs(t_side$nncos))*180/pi, t_side$area, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

hist(acos(abs(st$nncos))*180/pi, breaks=hist_breaks, xlim=c(0, max_angle), col=rgb(1, 1, 1, 1), xlab="Angle", main="Histograms of inter-normal angles for differed");
hist(acos(abs(st_stack$nncos))*180/pi, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
hist(acos(abs(st_side$nncos))*180/pi, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

weighted.hist(acos(abs(st$nncos))*180/pi, st$area, breaks=hist_breaks, xlim=c(0, max_angle), col=rgb(1, 1, 1, 1), xlab="Angle", main="Weighted histograms of inter-normal angles for differed");
weighted.hist(acos(abs(st_stack$nncos))*180/pi, st_stack$area, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
weighted.hist(acos(abs(st_side$nncos))*180/pi, st_side$area, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

########################

max_dist=15;
hist_bins=50;
hist_bin_size=(max_dist/hist_bins);
hist_breaks=(0:hist_bins)*hist_bin_size;
hist_x=hist_breaks[1:(length(hist_breaks)-1)];

hist(t$shift_dist, breaks=hist_breaks, xlim=c(0, max_dist), col=rgb(1, 1, 1, 1), xlab="Distance", main="Histograms of shift distances for all");
hist(t_stack$shift_dist, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
hist(t_side$shift_dist, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

weighted.hist(t$shift_dist, t$area, breaks=hist_breaks, xlim=c(0, max_dist), col=rgb(1, 1, 1, 1), xlab="Distance", main="Histograms of shift distances for all");
weighted.hist(t_stack$shift_dist, t_stack$area, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
weighted.hist(t_side$shift_dist, t_side$area, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

hist(st$shift_dist, breaks=hist_breaks, xlim=c(0, max_dist), col=rgb(1, 1, 1, 1), xlab="Distance", main="Histograms of shift distances for differed");
hist(st_stack$shift_dist, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
hist(st_side$shift_dist, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);

weighted.hist(st$shift_dist, st$area, breaks=hist_breaks, xlim=c(0, max_dist), col=rgb(1, 1, 1, 1), xlab="Distance", main="Histograms of shift distances for differed");
weighted.hist(st_stack$shift_dist, st_stack$area, breaks=hist_breaks, col=rgb(0, 1, 0, 0.45), add=TRUE);
weighted.hist(st_side$shift_dist, st_side$area, breaks=hist_breaks, col=rgb(0, 0, 1, 0.55), add=TRUE);
