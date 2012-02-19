t=read.table("reformatted_table", header=TRUE, stringsAsFactors=FALSE);
casp9_servers=read.table("../input_data/casp9_servers", header=FALSE, stringsAsFactors=FALSE);
casp9_targets_definition=read.table("../input_data/casp9_targets_definition", header=FALSE, stringsAsFactors=FALSE);
casp9_gdt_ha_table=read.table("../input_data/casp9_GDT_HA_table", header=TRUE, stringsAsFactors=FALSE);
casp9_interfaces_table=read.table("../input_data/casp9_inter_domains_interfaces_CAD_scores", header=TRUE, stringsAsFactors=FALSE);

########################

t=t[which(t$target!=629),];

########################

model_ratios=t$model_atoms_count/t$model_residues_count;
t=t[which(model_ratios<9.5),]
model_ratios=t$model_atoms_count/t$model_residues_count;
not_solid_real_groups=t$group[which(model_ratios<6.5)];
not_solid_real_groups_set=union(not_solid_real_groups, not_solid_real_groups);
solid_group=rep(1, length(t$group));
solid_group[which(is.element(t$group, not_solid_real_groups_set))]=0;
t=t[which(solid_group==1),];

########################

server=rep(0, length(t$group));
server[which(is.element(t$group, casp9_servers[[1]]))]=1;
t=t[which(server==1),];

########################

target_class=rep("undefined", length(t$target));
for(i in 1:length(casp9_targets_definition[[1]]))
{
  target_class[which((t$target*10+t$domain)==(casp9_targets_definition[[1]][i]*10+casp9_targets_definition[[2]][i]))]=casp9_targets_definition[[3]][i];
}
t$target_class=target_class;

########################

t$model_unique_id=((t$target*10+t$domain)*1000+t$group);

########################

CASP_GDT_HA=rep(-100, length(t$target));
casp9_gdt_ha_table$model_unique_id=((casp9_gdt_ha_table$target*10+casp9_gdt_ha_table$domain)*1000+casp9_gdt_ha_table$group);
for(i in 1:length(casp9_gdt_ha_table$target))
{
  sel=which(t$model_unique_id==casp9_gdt_ha_table$model_unique_id[i]);
  if(length(sel)==1)
  {
    CASP_GDT_HA[sel]=casp9_gdt_ha_table$CASP_GDT_HA[i];
  }
}
t$CASP_GDT_HA=CASP_GDT_HA

########################

interface_AA=rep(-1, length(t$target));
casp9_interfaces_table$model_unique_id=((casp9_interfaces_table$target*10+casp9_interfaces_table$domain)*1000+casp9_interfaces_table$group);
for(i in 1:length(casp9_interfaces_table$target))
{
  sel=which(t$model_unique_id==casp9_interfaces_table$model_unique_id[i]);
  if(length(sel)==1)
  {
    interface_AA[sel]=casp9_interfaces_table$AA[i];
  }
}
t$interface_AA=interface_AA

########################

t$LGA_GDT_TS=t$LGA_GDT_TS/100;
t$CASP_GDT_TS=t$CASP_GDT_TS/100;
t$CASP_GDT_HA=t$CASP_GDT_HA/100;

########################

write.table(t, "enhanced_table", quote=FALSE, row.names=FALSE);

########################

set_of_all_servers=casp9_servers[[1]];
set_of_used_servers=union(t$group, t$group);
set_of_unused_servers=setdiff(set_of_all_servers, set_of_used_servers);

list_of_used_servers=casp9_servers[which(is.element(casp9_servers[[1]], set_of_used_servers)),];
write.table(list_of_used_servers, "list_of_used_servers", quote=FALSE, row.names=FALSE, col.names=FALSE);

list_of_unused_servers=casp9_servers[which(is.element(casp9_servers[[1]], set_of_unused_servers)),];
write.table(list_of_unused_servers, "list_of_unused_servers", quote=FALSE, row.names=FALSE, col.names=FALSE);
