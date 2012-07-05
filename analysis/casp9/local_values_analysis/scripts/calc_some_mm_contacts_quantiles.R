a=read.table("some_alpha_targets_mm_contacts", header=FALSE);
sa=which(a[[1]]==99999)-1;
ap=a[sa,];
da=abs(ap[[1]]-ap[[2]]);


b=read.table("some_beta_targets_mm_contacts", header=FALSE);
sb=which(b[[1]]==99999)-1;
bp=b[sb,];
db=abs(bp[[1]]-bp[[2]]);

quantile(da, probs=(1:20)/20);

quantile(db, probs=(1:20)/20);

length(which(da>4))/length(da);

length(which(db>4))/length(da);
