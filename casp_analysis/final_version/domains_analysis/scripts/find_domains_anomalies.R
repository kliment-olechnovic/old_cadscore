t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);

found_targets=c();
found_groups1=c();
found_groups2=c();
found_GDT_TS1=c();
found_GDT_TS2=c();
found_AA1=c();
found_AA2=c();
found_interfaces_AA1=c();
found_interfaces_AA2=c();

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
              if(st1$interface_AA[1]>st2$interface_AA[1])
              {
                if(st1$AA[1]>st2$AA[1])
                {
                  if(st1$CASP_GDT_TS[1]<st2$CASP_GDT_TS[1])
                  {
                    true_counts=1;
                    for(j in 2:length(domains_set))
                    {
                      if(st1$AA[j]>st2$AA[j])
                      {
                        if(st1$CASP_GDT_TS[j]>st2$CASP_GDT_TS[j])
                        {
                          true_counts=true_counts+1;
                        }
                      }
                    }
                    if(true_counts==length(domains_set))
                    {
                      found_targets=c(found_targets, target);
                      found_groups1=c(found_groups1, g1);
                      found_groups2=c(found_groups2, g2);
                      found_GDT_TS1=c(found_GDT_TS1, st1$CASP_GDT_TS[1]);
                      found_GDT_TS2=c(found_GDT_TS2, st2$CASP_GDT_TS[1]);
                      found_AA1=c(found_AA1, st1$AA[1]);
                      found_AA2=c(found_AA2, st2$AA[1]);
                      found_interfaces_AA1=c(found_interfaces_AA1, st1$interface_AA[1]);
                      found_interfaces_AA2=c(found_interfaces_AA2, st2$interface_AA[1]);
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

result=data.frame(target=found_targets, group1=found_groups1, group2=found_groups2, GDT_TS_1=found_GDT_TS1, GDT_TS_2=found_GDT_TS2, AA_1=found_AA1, AA_2=found_AA2, interface_AA_1=found_interfaces_AA1, interface_AA_2=found_interfaces_AA2);

write.table(result, "domains_anomalies", quote=FALSE, row.names=FALSE);
