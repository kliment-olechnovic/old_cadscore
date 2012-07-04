t=read.table("casp9_targets_atoms", header=FALSE, stringsAsFactors=FALSE);
names=t[[2]];

ss_sel=which(names!="N");
ss_sel=intersect(ss_sel, which(names!="O"));
ss_sel=intersect(ss_sel, which(names!="C"));
ss_sel=intersect(ss_sel, which(names!="CA"));

ss_names=names[ss_sel];

ss_types=substr(ss_names, 1, 1);

summary=aggregate(rep(1, length(ss_types)), list(atom=ss_types), sum);

summary
