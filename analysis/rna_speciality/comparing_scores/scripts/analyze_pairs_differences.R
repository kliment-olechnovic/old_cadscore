rt=read.table("./output/decoys/pairs_differences_table.bak", header=TRUE, stringsAsFactors=FALSE);

rt=rt[which(abs(rt$diffs_AA)>0),];
rt=rt[which(abs(rt$diffs_SS)>0),];
rt=rt[which(abs(rt$diffs_rna_inf_norv)>0),];
rt=rt[which(abs(rt$diffs_rna_rmsd)>0),];
rt=rt[which(abs(rt$diffs_rna_di)>0),];
rt=rt[which(abs(rt$diffs_GDT_C3)>0),];
rt=rt[which(abs(rt$diffs_MP_clashscore)>0),];
rt=rt[which(abs(rt$diffs_MP_pct_badangles)>0),];

t=rt;

mp_signs_diffs=sign(t$diffs_MP_clashscore)-sign(t$diffs_MP_pct_badangles);
t=t[which(mp_signs_diffs==0),];

t=t[which(t$mins_rna_inf_norv>0.9),];
#t=t[which(t$maxs_rna_di<5),];

N=length(t[[1]]);
N
ids=1:N;

svs=data.frame(
		AA=sign(t$diffs_AA)*ids, 
		SS=sign(t$diffs_SS)*ids, 
		rna_inf_norv=sign(t$diffs_rna_inf_norv)*ids, 
		rna_rmsd=sign(t$diffs_rna_rmsd)*ids, 
		rna_di=sign(t$diffs_rna_di)*ids,
		GDT_C3=sign(t$diffs_GDT_C3)*ids,
		MP_clashscore=sign(t$diffs_MP_clashscore)*ids,
		MP_pct_badangles=sign(t$diffs_MP_pct_badangles)*ids);

##############

length(svs$SS);
length(intersect(svs$SS, svs$AA));
length(intersect(svs$SS, svs$rna_inf_norv));
length(intersect(svs$SS, svs$GDT_C3));
length(intersect(svs$SS, svs$rna_rmsd));
length(intersect(svs$SS, svs$rna_di));
length(intersect(svs$rna_inf_norv, svs$rna_rmsd));
length(intersect(svs$rna_inf_norv, svs$rna_di));
length(intersect(svs$rna_rmsd, svs$rna_di));

length(intersect(setdiff(svs$SS, svs$rna_inf_norv), svs$MP_clashscore));
length(intersect(setdiff(svs$rna_inf_norv, svs$SS), svs$MP_clashscore));

length(intersect(setdiff(svs$SS, svs$GDT_C3), svs$MP_clashscore));
length(intersect(setdiff(svs$GDT_C3, svs$SS), svs$MP_clashscore));

length(intersect(setdiff(svs$SS, svs$rna_rmsd), svs$MP_clashscore));
length(intersect(setdiff(svs$rna_rmsd, svs$SS), svs$MP_clashscore));

length(intersect(setdiff(svs$SS, svs$rna_di), svs$MP_clashscore));
length(intersect(setdiff(svs$rna_di, svs$SS), svs$MP_clashscore));
