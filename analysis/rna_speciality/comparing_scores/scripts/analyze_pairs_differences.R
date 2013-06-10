input_table_file="../collecting_scores/output/decoys/calculated_scores";
output_directory="./output/decoys";

t=read.table(input_table_file, header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$m_res_used/t$t_res_used>0.95),];
targets=union(t$target, t$target);

scorename_a="rna_rmsd";
scorename_b="SS";
sel_size=120;

diffs_a=c();
diffs_b=c();
lefts=c();
rights=c();

for(target in targets)
{
st=t[which(t$target==target),];
sel_ai=rev(order(st[, scorename_a]))[1:sel_size];
sel_aj=order(st[, scorename_a])[1:sel_size];
sel_b=rev(order(st[, scorename_b]))[1:sel_size];
sti=st[intersect(sel_ai, sel_b),];
stj=st[intersect(sel_aj, sel_b),];
if(length(sti[[1]])>0 && length(stj[[1]])>0)
{
for(i in 1:length(sti[[1]]))
{
  for(j in 1:length(stj[[1]]))
  {
    diffs_a=c(diffs_a, sti[i, scorename_a]-stj[j, scorename_a]);
    diffs_b=c(diffs_b, sti[i, scorename_b]-stj[j, scorename_b]);
    lefts=c(lefts, sti[i, "model"]);
    rights=c(rights, stj[j, "model"]);
  }
}
}
}

plot(diffs_a, diffs_b);

sel=intersect(which(diffs_a>4), which(abs(diffs_b)<0.01));
data.frame(left=lefts[sel], right=rights[sel]);
