t=read.table("casp9_combined_contacts", header=FALSE);

ta=t[[2]];
ma=t[[3]];
cad=abs(ta-ma);

probs_list=c(0, 0.05, 0.1, 0.5, 0.9, 0.95, 1.0);

quantile(ta, probs=probs_list);

quantile(ma, probs=probs_list);

quantile(cad, probs=probs_list);
