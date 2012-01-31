rt=read.table("pairs_differences_table", header=TRUE, stringsAsFactors=FALSE);
rt=rt[which(rt$MolProbity!=0),];
rt=rt[which(rt$GDT_TS!=0),];
rt=rt[which(rt$SS!=0),];

names=c("MP", "GDT", "AA", "SA", "SS");
names=c(names, "MP and GDT", "MP and AA", "MP and SA", "MP and SS");
names=c(names, "GDT and AA", "GDT and SA", "GDT and SS");
names=c(names, "MP and AA and GDT", "MP and SA and GDT", "MP and SS and GDT");
names=c(names, "MP and AA without GDT", "MP and GDT without AA", "MP and SA without GDT", "MP and GDT without SA", "MP and SS without GDT", "MP and GDT without SS");
names=c(names, "MP without GDT without AA", "MP without GDT without SA", "MP without GDT without SS");

llt=data.frame(set_name=names);

tresholds_GDT_TS=c(0.6, 0.7, 0.8, 0.9);
for(treshold in tresholds_GDT_TS)
{
  t=rt[which(rt$min_GDT_TS>=treshold),];
  ids=1:length(t[[1]]);
  M=sign(t$MolProbity)*ids;
  G=sign(t$GDT_TS)*ids;
  AA=sign(t$AA)*ids;
  SA=sign(t$SA)*ids;
  SS=sign(t$SS)*ids;
  
  ll=c();
  ll=c(ll, length(M));
  ll=c(ll, length(G));
  ll=c(ll, length(AA));
  ll=c(ll, length(SA));
  ll=c(ll, length(SS));
  
  ll=c(ll, length(intersect(M, G)));
  ll=c(ll, length(intersect(M, AA)));
  ll=c(ll, length(intersect(M, SA)));
  ll=c(ll, length(intersect(M, SS)));
  
  ll=c(ll, length(intersect(G, AA)));
  ll=c(ll, length(intersect(G, SA)));
  ll=c(ll, length(intersect(G, SS)));
  
  ll=c(ll, length(intersect(intersect(M, AA), G)));
  ll=c(ll, length(intersect(intersect(M, SA), G)));
  ll=c(ll, length(intersect(intersect(M, SS), G)));
  
  ll=c(ll, length(setdiff(intersect(M, AA), G)));
  ll=c(ll, length(setdiff(intersect(M, G), AA)));
  ll=c(ll, length(setdiff(intersect(M, SA), G)));
  ll=c(ll, length(setdiff(intersect(M, G), SA)));
  ll=c(ll, length(setdiff(intersect(M, SS), G)));
  ll=c(ll, length(setdiff(intersect(M, G), SS)));
  
  ll=c(ll, length(setdiff(M, union(G, AA))));
  ll=c(ll, length(setdiff(M, union(G, SA))));
  ll=c(ll, length(setdiff(M, union(G, SS))));
  
  llt[, paste("GDT_TS_", (treshold*100), sep="")]=ll;
}

write.table(llt, "agreement_table", quote=TRUE, row.names=FALSE);
