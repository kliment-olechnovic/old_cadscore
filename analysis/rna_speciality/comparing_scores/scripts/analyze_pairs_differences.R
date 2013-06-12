cst=read.table("../collecting_scores/output/decoys/calculated_scores", header=TRUE, stringsAsFactors=FALSE);
cst=cst[which(cst$m_res_used/cst$t_res_used>0.95),];
mst=read.table("../collecting_scores/input/decoys_molprobity_scores", header=TRUE, stringsAsFactors=FALSE);

t=merge(cst, mst);

t=t[which(is.finite(t$rna_rmsd)),];
t=t[which(is.finite(t$rna_inf_norv)),];
t$rna_di=t$rna_rmsd/t$rna_inf_norv;
t=t[which(is.finite(t$rna_di)),];

length(t[[1]]);
t=t[which(t$rna_rmsd<2),];
length(t[[1]]);

targets=union(t$target, t$target);

mins_rna_rmsd=c();
diffs_AA=c();
diffs_SS=c();
diffs_rna_inf_norv=c();
diffs_rna_rmsd=c();
diffs_rna_di=c();
diffs_MP_clashscore=c();
diffs_MP_pct_badangles=c();

for(target in targets)
{
st=t[which(t$target==target),];
N=length(st[[1]]);
for(i in 1:N)
{
  for(j in (i+1):N)
  {
	mins_rna_rmsd=c(mins_rna_rmsd, min(st$rna_rmsd[i], st$rna_rmsd[j]));
	diffs_AA=c(diffs_AA, st$AA[i]-st$AA[j]);
	diffs_SS=c(diffs_SS, st$SS[i]-st$SS[j]);
	diffs_rna_inf_norv=c(diffs_rna_inf_norv, st$rna_inf_norv[i]-st$rna_inf_norv[j]);
	diffs_rna_rmsd=c(diffs_rna_rmsd, st$rna_rmsd[j]-st$rna_rmsd[i]);
	diffs_rna_di=c(diffs_rna_di, st$rna_di[j]-st$rna_di[i]);
	diffs_MP_clashscore=c(diffs_MP_clashscore, st$MP_clashscore[j]-st$MP_clashscore[i]);
	diffs_MP_pct_badangles=c(diffs_MP_pct_badangles, st$MP_pct_badangles[j]-st$MP_pct_badangles[i]);
  }
}
}

length(diffs_AA);

ids=1:length(diffs_AA);
vs=data.frame(
  m_rna_rmsd=mins_rna_rmsd,
  d_AA=diffs_AA, 
  d_SS=diffs_SS, 
  d_rna_inf_norv=diffs_rna_inf_norv, 
  d_rna_rmsd=diffs_rna_rmsd, 
  d_rna_di=diffs_rna_di, 
  d_MP_clashscore=diffs_MP_clashscore,
  d_MP_pct_badangles=diffs_MP_pct_badangles,
  v_AA=sign(diffs_AA)*ids, 
  v_SS=sign(diffs_SS)*ids, 
  v_rna_inf_norv=sign(diffs_rna_inf_norv)*ids, 
  v_rna_rmsd=sign(diffs_rna_rmsd)*ids, 
  v_rna_di=sign(diffs_rna_di)*ids, 
  v_MP_clashscore=sign(diffs_MP_clashscore)*ids,
  v_MP_pct_badangles=sign(diffs_MP_pct_badangles)*ids);

write.table(vs, "pairs_differences_table", quote=FALSE, row.names=FALSE);

##############

svs=vs;
svs=svs[which(svs$m_rna_rmsd<1),];
svs=svs[which(abs(svs$v_MP_clashscore)>1),];

length(setdiff(intersect(svs$v_MP_clashscore, svs$v_SS), svs$v_rna_inf_norv));
length(setdiff(intersect(svs$v_MP_clashscore, svs$v_rna_inf_norv), svs$v_SS));

length(setdiff(intersect(svs$v_MP_clashscore, svs$v_SS), svs$v_rna_rmsd));
length(setdiff(intersect(svs$v_MP_clashscore, svs$v_rna_rmsd), svs$v_SS));

length(setdiff(intersect(svs$v_MP_clashscore, svs$v_SS), svs$v_rna_di));
length(setdiff(intersect(svs$v_MP_clashscore, svs$v_rna_di), svs$v_SS));

##############

svs=vs;
svs=svs[which(svs$m_rna_rmsd<1),];
svs=svs[which(abs(svs$v_MP_pct_badangles)>1),];

length(setdiff(intersect(svs$v_MP_pct_badangles, svs$v_SS), svs$v_rna_inf_norv));
length(setdiff(intersect(svs$v_MP_pct_badangles, svs$v_rna_inf_norv), svs$v_SS));

length(setdiff(intersect(svs$v_MP_pct_badangles, svs$v_SS), svs$v_rna_rmsd));
length(setdiff(intersect(svs$v_MP_pct_badangles, svs$v_rna_rmsd), svs$v_SS));

length(setdiff(intersect(svs$v_MP_pct_badangles, svs$v_SS), svs$v_rna_di));
length(setdiff(intersect(svs$v_MP_pct_badangles, svs$v_rna_di), svs$v_SS));
