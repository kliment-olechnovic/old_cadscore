rt=read.table("./output/decoys/pairs_differences_table.bak", header=TRUE, stringsAsFactors=FALSE);

rt=rt[which(abs(rt$diffs_AA)>0),];
rt=rt[which(abs(rt$diffs_SS)>0),];
rt=rt[which(abs(rt$diffs_rna_inf_norv)>0),];
rt=rt[which(abs(rt$diffs_rna_rmsd)>0),];
rt=rt[which(abs(rt$diffs_rna_di)>0),];
rt=rt[which(abs(rt$diffs_GDT_C3)>0),];
rt=rt[which(abs(rt$diffs_MP_clashscore)>0),];
rt=rt[which(abs(rt$diffs_MP_pct_badangles)>0),];
rt=rt[which(rt$maxs_rna_rmsd<5),];

mp_signs_diffs=sign(rt$diffs_MP_clashscore)-sign(rt$diffs_MP_pct_badangles);
rt=rt[which(mp_signs_diffs==0),];

##############

t=rt;
t=t[which(t$mins_rna_inf_norv>0.5),];

N=length(t[[1]]);
ids=1:N;

AA=sign(t$diffs_AA)*ids;
SS=sign(t$diffs_SS)*ids;
rna_inf_norv=sign(t$diffs_rna_inf_norv)*ids;
rna_rmsd=sign(t$diffs_rna_rmsd)*ids;
rna_di=sign(t$diffs_rna_di)*ids;
GDT_C3=sign(t$diffs_GDT_C3)*ids;
MP_clashscore=sign(t$diffs_MP_clashscore)*ids;
MP_pct_badangles=sign(t$diffs_MP_pct_badangles)*ids;

##############

length(SS);
length(intersect(SS, AA));
length(intersect(SS, rna_inf_norv));
length(intersect(SS, GDT_C3));
length(intersect(SS, rna_rmsd));
length(intersect(SS, rna_di));
length(intersect(rna_inf_norv, rna_rmsd));
length(intersect(rna_inf_norv, rna_di));
length(intersect(rna_rmsd, rna_di));

length(intersect(setdiff(SS, rna_inf_norv), MP_clashscore));
length(intersect(setdiff(rna_inf_norv, SS), MP_clashscore));

length(intersect(setdiff(SS, GDT_C3), MP_clashscore));
length(intersect(setdiff(GDT_C3, SS), MP_clashscore));

length(intersect(setdiff(SS, rna_rmsd), MP_clashscore));
length(intersect(setdiff(rna_rmsd, SS), MP_clashscore));

length(intersect(setdiff(SS, rna_di), MP_clashscore));
length(intersect(setdiff(rna_di, SS), MP_clashscore));
