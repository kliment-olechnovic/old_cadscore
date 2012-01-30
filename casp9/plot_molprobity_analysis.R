t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$MolProbityScore>(0-1)),];
t=t[which(t$domain>0),];

output_directory="plot_molprobity_analysis";
dir.create(output_directory);

targets_set=union(t$target, t$target);

cadscore_name="SA";

gdt_signs=c();
cadscore_signs=c();
molprobity_signs=c();

for(target in targets_set)
{
  st=t[which(t$target==target),];
  N=length(st[[1]]);
  for(i in 1:(N-1))
  {
    for(j in (i+1):N)
    {
      gdt_signs=c(gdt_signs, sign(st$CASP_GDT_TS[i]-st$CASP_GDT_TS[j]));
      cadscore_signs=c(cadscore_signs, sign(st[i, cadscore_name]-st[j, cadscore_name]));
      molprobity_signs=c(molprobity_signs, sign(st$MolProbityScore[j]-st$MolProbityScore[i]));
    }
  }
}

sel=which((molprobity_signs*gdt_signs*cadscore_signs)!=0);
gdt_signs=gdt_signs[sel];
cadscore_signs=cadscore_signs[sel];
molprobity_signs=molprobity_signs[sel];

ids=1:length(gdt_signs);
A=molprobity_signs*ids;
B=cadscore_signs*ids;
C=gdt_signs*ids;

A_and_B_without_C=setdiff(intersect(A, B), C);
A_and_C_without_B=setdiff(intersect(A, C), B);

slices=c(length(A_and_B_without_C), length(A_and_C_without_B));
situations=c(paste(cadscore_name, " agrees with MolProbity, but disagrees with GDT_TS", sep=""), paste("GDT_TS agrees with MolProbity, but disagrees with ", cadscore_name, sep=""));
png(paste(output_directory, "/", "pie", "_", cadscore_name, ".png", sep=""), height=5, width=12, units="in", res=200);
pie(slices, labels=situations, col=rainbow(length(slices)), main=paste("Pie for ", cadscore_name, sep=""));
dev.off();
