command_args=commandArgs(TRUE);

constraint_mode="0";
if(length(command_args)==1)
{
  constraint_mode=command_args[1];
}

rt=read.table("pairs_differences_table", header=TRUE, stringsAsFactors=FALSE);
rt=rt[which(rt$MolProbity!=0),];
rt=rt[which(rt$GDT_TS!=0),];
rt=rt[which(rt$SS!=0),];

if(constraint_mode=="sd")
{
  rt=rt[which(abs(rt$MolProbity)>=sd(rt$MolProbity)),];
}
if(constraint_mode=="sd_or")
{
	rt=rt[which(abs(rt$MolProbity)>=sd(rt$MolProbity)),];
	selection=which(abs(rt$GDT_TS)>=sd(rt$GDT_TS));
	selection=union(selection, which(abs(rt$AA)>=sd(rt$AA)));
	rt=rt[selection,];
}
if(constraint_mode=="sd_and")
{
	rt=rt[which(abs(rt$MolProbity)>=sd(rt$MolProbity)),];
	selection=which(abs(rt$GDT_TS)>=sd(rt$GDT_TS));
	selection=intersect(selection, which(abs(rt$AA)>=sd(rt$AA)));
	rt=rt[selection,];
}
if(constraint_mode=="sd_gdt")
{
	rt=rt[which(abs(rt$MolProbity)>=sd(rt$MolProbity)),];
	selection=which(abs(rt$GDT_TS)>=sd(rt$GDT_TS));
	rt=rt[selection,];
}
if(constraint_mode=="sd_cad")
{
	rt=rt[which(abs(rt$MolProbity)>=sd(rt$MolProbity)),];
	selection=which(abs(rt$AA)>=sd(rt$AA));
	rt=rt[selection,];
}
if(constraint_mode=="0")
{
  rt=rt[which(abs(rt$MolProbity)>=0),];
}
if(constraint_mode=="1")
{
  rt=rt[which(abs(rt$MolProbity)>=1),];
}

names=c("MP", "GDT", "AA", "AS", "SS");
names=c(names, "MP and GDT", "MP and AA", "MP and AS", "MP and SS");
names=c(names, "GDT and AA", "GDT and AS", "GDT and SS");
names=c(names, "MP and AA and GDT", "MP and AS and GDT", "MP and SS and GDT");
names=c(names, "MP and AA without GDT", "MP and GDT without AA", "MP and AS without GDT", "MP and GDT without AS", "MP and SS without GDT", "MP and GDT without SS");
names=c(names, "MP without GDT without AA", "MP without GDT without AS", "MP without GDT without SS");

llt=data.frame(set_name=names);

tresholds_GDT_TS=c(0.6, 0.7, 0.8, 0.9);
for(treshold in tresholds_GDT_TS)
{
  t=rt[which(rt$min_GDT_TS>=treshold),];
  ids=1:length(t[[1]]);
  M=sign(t$MolProbity)*ids;
  G=sign(t$GDT_TS)*ids;
  AA=sign(t$AA)*ids;
  AS=sign(t$AS)*ids;
  SS=sign(t$SS)*ids;
  
  ll=c();
  ll=c(ll, length(M));
  ll=c(ll, length(G));
  ll=c(ll, length(AA));
  ll=c(ll, length(AS));
  ll=c(ll, length(SS));
  
  ll=c(ll, length(intersect(M, G)));
  ll=c(ll, length(intersect(M, AA)));
  ll=c(ll, length(intersect(M, AS)));
  ll=c(ll, length(intersect(M, SS)));
  
  ll=c(ll, length(intersect(G, AA)));
  ll=c(ll, length(intersect(G, AS)));
  ll=c(ll, length(intersect(G, SS)));
  
  ll=c(ll, length(intersect(intersect(M, AA), G)));
  ll=c(ll, length(intersect(intersect(M, AS), G)));
  ll=c(ll, length(intersect(intersect(M, SS), G)));
  
  ll=c(ll, length(setdiff(intersect(M, AA), G)));
  ll=c(ll, length(setdiff(intersect(M, G), AA)));
  ll=c(ll, length(setdiff(intersect(M, AS), G)));
  ll=c(ll, length(setdiff(intersect(M, G), AS)));
  ll=c(ll, length(setdiff(intersect(M, SS), G)));
  ll=c(ll, length(setdiff(intersect(M, G), SS)));
  
  ll=c(ll, length(setdiff(M, union(G, AA))));
  ll=c(ll, length(setdiff(M, union(G, AS))));
  ll=c(ll, length(setdiff(M, union(G, SS))));
  
  llt[, paste("GDT_TS_", (treshold*100), sep="")]=ll;
}

output_directory=paste("judging_from_", constraint_mode, sep="")
dir.create(output_directory);

write.table(llt, paste(output_directory, "/agreement_table", sep=""), quote=TRUE, row.names=FALSE);

#######################################

t=rt[which(rt$min_GDT_TS>=0.6),];
ids=1:length(t[[1]]);
M=sign(t$MolProbity)*ids;
G=sign(t$GDT_TS)*ids;
AA=sign(t$AA)*ids;
write.table(t[abs(setdiff(intersect(M, G), AA)),], paste(output_directory, "/unsupportive_pairs_differences", sep=""), quote=TRUE, row.names=FALSE);
