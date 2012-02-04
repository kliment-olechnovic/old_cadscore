t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);

output_directory="plot_domains_second_analysis";
dir.create(output_directory);

score_names=c("LGA_GDT_TS", "AA", "SA", "SS");
for(score_name in score_names)
{

targets_set=t$target[which(t$domain==0)];
targets_set=union(targets_set, targets_set);

target_map=c();
full_scores=c();
combined_scores=c();
full_sas_values=c();
split_sas_values=c();
min_domain_GDT_TS=c();

for(target in targets_set)
{
  st=t[which(t$target==target),];
  target_domains_count=length(union(st$domain, st$domain));
  if(target_domains_count>2)
  {
    groups_set=union(st$group, st$group);
    for(group in groups_set)
    {
      sst=st[which(st$group==group),];
      group_domains_count=length(sst$domain);
      if(group_domains_count==target_domains_count)
      {
        full_score=0;
        full_sas=0;
        weighted_sum=0;
        sum_of_weights=0;
        split_sas=0;
        for(j in 1:group_domains_count)
        {
          if(sst$domain[j]==0)
          {
            full_score=sst[, score_name][j];
            full_sas=sst$AW_ref[j];
          }
          else
          {
            weighted_sum=weighted_sum+(sst[, score_name][j]*sst$target_atoms_count[j]);
            sum_of_weights=sum_of_weights+sst$target_atoms_count[j];
            split_sas=split_sas+sst$AW_ref[j];
          }
        }
        target_map=c(target_map, target);
        full_scores=c(full_scores, full_score);
        full_sas_values=c(full_sas_values, full_sas);
        combined_score=weighted_sum/sum_of_weights;
        combined_scores=c(combined_scores, combined_score);
        split_sas_values=c(split_sas_values, split_sas);
        min_domain_GDT_TS=c(min_domain_GDT_TS, min(sst$CASP_GDT_TS[which(sst$domain>0)]));
      }
    }
  }
}

png(paste(output_directory, "/", "combination", "_", score_name, ".png", sep=""), height=7, width=7, units="in", res=200);
plot(x=full_scores, y=combined_scores, xlim=c(0, 1), ylim=c(0, 1), col="black", cex=0.5, xlab="Full model score", ylab="Combined domains score", main=score_name);
points(x=c(0, 1), y=c(0, 1), type="l", lwd=1);
dev.off();

tresholds_GDT_TS=c(0.0, 0.2, 0.4, 0.6, 0.8);
for(treshold in tresholds_GDT_TS)
{
  treshold_sel=which(min_domain_GDT_TS>=treshold);
  treshold_target_map=target_map[treshold_sel];
  
  orientation_values=(split_sas_values-full_sas_values)/2/split_sas_values*full_scores;
  difference_values=abs(full_scores-combined_scores);
  
  orientation_values=orientation_values[treshold_sel];
  difference_values=difference_values[treshold_sel];

  targets_set=union(treshold_target_map, treshold_target_map);
  orientation_means=c();
  difference_means=c();
  difference_minimums=c();
  difference_maximums=c();
  for(target in targets_set)
  {
    sel=which(treshold_target_map==target);
    orientation_means=c(orientation_means, mean(orientation_values[sel]));
    difference_means=c(difference_means, mean(difference_values[sel]));
    difference_minimums=c(difference_minimums, min(difference_values[sel]));
    difference_maximums=c(difference_maximums, max(difference_values[sel]));
  }

  png(paste(output_directory, "/", "interface", "_", score_name, "_treshold_", (treshold*100), ".png", sep=""), height=7, width=7, units="in", res=200);
  ord=order(orientation_values);
  plot(x=orientation_values[ord], y=difference_values[ord], type="n", xlab="Expected absolute difference", ylab="abs((Full model score)-(Combined domains score))", main=paste(score_name, ", GDT_TS>", treshold, sep=""));
  points(x=orientation_values[ord], y=difference_values[ord], col="black", cex=0.5);
  ord=order(orientation_means);
  points(x=orientation_means[ord], y=difference_means[ord], col="purple", type="l", lwd=2);
  #points(x=orientation_means[ord], y=difference_minimums[ord], col="blue", type="l", lwd=2);
  #points(x=orientation_means[ord], y=difference_maximums[ord], col="red", type="l", lwd=2);
  dev.off();
}

}
