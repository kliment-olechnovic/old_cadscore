t=read.table("./output/decoys/merged_scores", header=TRUE, stringsAsFactors=FALSE);

t=t[which(t$m_res_used/t$t_res_used>0.99),];

rt=read.table("./input/decoys_targets_molprobity_scores", header=TRUE, stringsAsFactors=FALSE);

targets=union(t$target, t$target);
positions=c();

for(target in targets)
{
	st=t[which(t$target==target),];
	srt=rt[which(rt$target==target),];
	positions=c(positions, length(which(st$MP_clashscore<srt$MP_clashscore[1])));
}

good_targets=targets[which(positions==0)];

length(targets);
length(good_targets);

good_t=t[which(is.element(t$target, good_targets)),];

write.table(good_t, "./output/decoys/refined_merged_scores", quote=FALSE, row.names=FALSE, col.names=TRUE);
