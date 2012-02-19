t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
i=read.table("casp9_initerfaces_table", header=TRUE, stringsAsFactors=FALSE);

####################

st=t[which(t$target==533),];
st1=st[which(st$group==453),];
st1=st1[order(st1$domain),];
st2=st[which(st$group==245),];
st2=st2[order(st2$domain),];

si=i[which(i$target==533),];
si1=si[which(si$group==453),];
si2=si[which(si$group==245),];

st1$AA;si1$sm0_AA;st1$CASP_GDT_TS;st1$LGA_GDT_TS;st1$TM_score_GDT_TS;
st2$AA;si2$sm0_AA;st2$CASP_GDT_TS;st2$LGA_GDT_TS;st2$TM_score_GDT_TS;

####################

score_name="CASP_GDT_TS";

found_targets=c();
found_groups1=c();
found_groups2=c();

targets_set=union(t$target[which(t$domain==2)], t$target[which(t$domain==2)]);
for(target in targets_set)
{
  st=t[which(t$target==target),];
  domains_set=sort(union(st$domain, st$domain));
  if(length(domains_set)>2)
  {
    groups_set=sort(union(st$group, st$group));
    for(g1 in groups_set)
    {
      st1=st[which(st$group==g1),];
      st1=st1[order(st1$domain),];
      if(length(union(st1$domain, st1$domain))==length(domains_set))
      {
        for(g2 in groups_set)
        {
          if(g1!=g2)
          {
            st2=st[which(st$group==g2),];
            st2=st2[order(st2$domain),];
            if(length(union(st2$domain, st2$domain))==length(domains_set))
            {
              true_counts=0;
              if(st1[1, score_name]<=st2[1, score_name])
              {
                true_counts=true_counts+1;
              }
              for(j in 2:length(domains_set))
              {
                if(st1[j, score_name]>st2[j, score_name])
                {
                  true_counts=true_counts+1;
                }
              }
              if(true_counts==length(domains_set))
              {
                found_targets=c(found_targets, target);
                found_groups1=c(found_groups1, g1);
                found_groups2=c(found_groups2, g2);
              }
            }
          }
        }
      }
    }
  }
}
