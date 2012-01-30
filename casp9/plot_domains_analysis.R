t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);

score_name="AA";

targets_set=t$target[which(t$domain==0)];
targets_set=union(targets_set, targets_set);

full_scores=c();
combined_scores=c();
full_sas_values=c();
split_sas_values=c();

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
	full_scores=c(full_scores, full_score);
	full_sas_values=c(full_sas_values, full_sas);
	combined_score=weighted_sum/sum_of_weights;
	combined_scores=c(combined_scores, combined_score);
	split_sas_values=c(split_sas_values, split_sas);
      }
    }
  }
}

plot(full_scores, combined_scores);

plot(x=(split_sas_values-full_sas_values)/split_sas_values, y=(full_scores-combined_scores));
