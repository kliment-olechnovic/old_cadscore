t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$MolProbityScore>(0-1)),];
t=t[which(t$domain>0),];

output_directory="plot_molprobity_analysis";
dir.create(output_directory);

cadscore_name="SA";

targets_set=union(t$target, t$target);

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

venn_parameters_1=c(length(A), length(B), length(C), length(intersect(A, B)), length(intersect(A, C)), length(intersect(B, C)), length(intersect(intersect(A, B), C)));
write(venn_parameters_1, paste(output_directory, "/", "venn_parameters_1", sep=""), ncolumns=1);

sel=which((gdt_signs*cadscore_signs)<0);
A=A[sel];
B=B[sel];
C=C[sel];
venn_parameters_2=c(length(A), length(B), length(C), length(intersect(A, B)), length(intersect(A, C)), length(intersect(B, C)), length(intersect(intersect(A, B), C)));
write(venn_parameters_2, paste(output_directory, "/", "venn_parameters_2", sep=""), ncolumns=1);
