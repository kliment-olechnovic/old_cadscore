cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE);

########################

t=t[which(t$model_residues_count>0),];
t=t[which(t$target_residues_count>0),];

########################

CASP=rep(8, length(t$target));
CASP[which(t$target>514)]=9;
t$CASP=CASP;

real_groups=(CASP*1000)+t$group;

########################

casp_8_server_groups=read.table("casp_8_servers.txt", header=FALSE);
casp_9_server_groups=read.table("casp_9_servers.txt", header=FALSE);
casp_8_server_real_groups=casp_8_server_groups[[1]]+8000;
casp_9_server_real_groups=casp_9_server_groups[[1]]+9000;
server_real_groups=c(casp_8_server_real_groups, casp_9_server_real_groups);

server=rep(0, length(real_groups));
server[which(is.element(real_groups, server_real_groups))]=1;
t$server=server;

########################

target_ratios=t$target_atoms_count/t$target_residues_count;
min_target_ratio=min(target_ratios);
model_ratios=t$model_atoms_count/t$model_residues_count;

not_solid_real_groups=real_groups[which(model_ratios<min_target_ratio)];
not_solid_real_groups_set=union(not_solid_real_groups, not_solid_real_groups);

solid_group=rep(1, length(real_groups));
solid_group[which(is.element(real_groups, not_solid_real_groups_set))]=0;
t$solid_group=solid_group;

########################

targets_set=union(t$target, t$target);
target_parts_count=rep(0, length(t$target));
for(i in targets_set)
{
  i_domains=t$domain[which(t$target==i)];
  i_domains_set=union(i_domains, i_domains);
  target_parts_count[which(t$target==i)]=length(i_domains_set);
}
t$target_parts_count=target_parts_count;

########################

casp_9_targets_definition=read.table("casp_9_targets_definition.txt", header=FALSE, stringsAsFactors=FALSE);
target_class=rep("unknown", length(t$target));
for(i in 1:length(casp_9_targets_definition[[1]]))
{
  target_class[which(t$target==casp_9_targets_definition[[1]][i])]=casp_9_targets_definition[[2]][i];
}
t$target_class=target_class;

########################

write.table(t, cmd_args[2], quote=FALSE, row.names=FALSE);
