input_table_file="../collecting_scores/output/rp/merged_scores";
output_directory="./output/rp";

t=read.table(input_table_file, header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$m_res_used/t$t_res_used>0.95),];
t=t[which(t$target!="4_solution_0"),];
targets=union(t$target, t$target);

scorename_a="ext_rmsd";
scorename_b="SS";

diffs_a=c();
diffs_b=c();
lefts=c();
rights=c();

for(target in targets)
{
st=t[which(t$target==target),];
for(i in 1:length(st[[1]]))
{
  for(j in (i+1):length(st[[1]]))
  {
    diffs_a=c(diffs_a, st[i, scorename_a]-st[j, scorename_a]);
    diffs_b=c(diffs_b, st[i, scorename_b]-st[j, scorename_b]);
    lefts=c(lefts, st[i, "model"]);
    rights=c(rights, st[j, "model"]);
  }
}
}

plot(diffs_a, diffs_b);

sel=intersect(which(diffs_a>10), which(diffs_b>0));
data.frame(left=lefts[sel], right=rights[sel]);
