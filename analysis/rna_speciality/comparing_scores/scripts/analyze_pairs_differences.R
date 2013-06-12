rt=read.table("./output/decoys/pairs_differences_table.bak", header=TRUE, stringsAsFactors=FALSE);

t=rt;

N=length(t[[1]]);
ids=1:N;

vs=data.frame(
	AA=sign(t$diffs_AA)*ids, 
	SS=sign(t$diffs_SS)*ids, 
	rna_inf_norv=sign(t$diffs_rna_inf_norv)*ids, 
	rna_rmsd=sign(t$diffs_rna_rmsd)*ids, 
	rna_di=sign(t$diffs_rna_di)*ids, 
	MP_clashscore=sign(t$diffs_MP_clashscore)*ids,
	MP_pct_badangles=sign(t$diffs_MP_pct_badangles)*ids);

##############

sel=which(abs(t$diffs_rna_inf_norv)>0.0);
sel=intersect(sel, which(abs(t$diffs_SS)>0.0));
svs=vs[sel,];

length(setdiff(intersect(svs$MP_clashscore, svs$SS), svs$rna_inf_norv));
length(setdiff(intersect(svs$MP_clashscore, svs$rna_inf_norv), svs$SS));

length(setdiff(intersect(svs$MP_clashscore, svs$SS), svs$rna_rmsd));
length(setdiff(intersect(svs$MP_clashscore, svs$rna_rmsd), svs$SS));

length(setdiff(intersect(svs$MP_clashscore, svs$SS), svs$rna_di));
length(setdiff(intersect(svs$MP_clashscore, svs$rna_di), svs$SS));
