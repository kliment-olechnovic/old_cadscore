rt=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
rt=rt[which(rt$MolProbityScore>(0-1)),];
rt=rt[which(rt$domain>0),];

output_directory="plot_molprobity_analysis";
dir.create(output_directory);

tresholds_GDT_TS=c(0.6, 0.7, 0.8, 0.9);
for(treshold in tresholds_GDT_TS)
{
  t=rt[which(rt$CASP_GDT_TS>=treshold),];
  
  targets_set=union(t$target, t$target);

  cadscore_names=c("AA", "SA", "SS");
  for(cadscore_name in cadscore_names)
  {
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
    png(paste(output_directory, "/", "pie_", cadscore_name, "_treshold_", (treshold*100), ".png", sep=""), height=5, width=12, units="in", res=200);
    pie(slices, labels=situations, col=rainbow(length(slices)), main=paste("Pie for ", cadscore_name, ", GDT_TS>", treshold, sep=""));
    dev.off();
  }

}
