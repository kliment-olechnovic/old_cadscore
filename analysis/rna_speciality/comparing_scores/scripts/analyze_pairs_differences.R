rt=read.table("./output/decoys/pairs_differences_table.bak", header=TRUE, stringsAsFactors=FALSE);

t=rt;
t=t[which(t$mins_rna_rmsd<1),];
t=t[which(abs(t$diffs_MP_clashscore)>1),];
t=t[which(abs(t$diffs_MP_pct_badangles)>1),];

N=length(t[[1]]);
N
ids=1:N;

svs=data.frame(
	AA=sign(t$diffs_AA)*ids, 
	SS=sign(t$diffs_SS)*ids, 
	rna_inf_norv=sign(t$diffs_rna_inf_norv)*ids, 
	rna_rmsd=sign(t$diffs_rna_rmsd)*ids, 
	rna_di=sign(t$diffs_rna_di)*ids, 
	MP_clashscore=sign(t$diffs_MP_clashscore)*ids,
	MP_pct_badangles=sign(t$diffs_MP_pct_badangles)*ids);

##############

length(intersect(setdiff(svs$SS, svs$rna_inf_norv), svs$MP_clashscore));
length(intersect(setdiff(svs$rna_inf_norv, svs$SS), svs$MP_clashscore));

length(intersect(setdiff(svs$SS, svs$rna_rmsd), svs$MP_clashscore));
length(intersect(setdiff(svs$rna_rmsd, svs$SS), svs$MP_clashscore));

length(intersect(setdiff(svs$SS, svs$rna_di), svs$MP_clashscore));
length(intersect(setdiff(svs$rna_di, svs$SS), svs$MP_clashscore));

##############

length(intersect(setdiff(svs$SS, svs$rna_inf_norv), svs$MP_pct_badangles));
length(intersect(setdiff(svs$rna_inf_norv, svs$SS), svs$MP_pct_badangles));

length(intersect(setdiff(svs$SS, svs$rna_rmsd), svs$MP_pct_badangles));
length(intersect(setdiff(svs$rna_rmsd, svs$SS), svs$MP_pct_badangles));

length(intersect(setdiff(svs$SS, svs$rna_di), svs$MP_pct_badangles));
length(intersect(setdiff(svs$rna_di, svs$SS), svs$MP_pct_badangles));
