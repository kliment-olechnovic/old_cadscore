cmd_args=commandArgs(TRUE);

t=read.table(cmd_args[1], header=TRUE, stringsAsFactors=FALSE);
t=t[which(t$target_parts_count==3),];
t=t[which(t$solid_group==1),];

tid=read.table(cmd_args[2], header=TRUE, stringsAsFactors=FALSE);

tid_keys=tid$target*10000+tid$group*10+tid$domain;

t_keys=t$target*10000+t$group*10+t$domain;
t0_keys=t_keys[which(is.element(t_keys, tid_keys))];
t1_keys=t_keys[which(is.element(t_keys, (t0_keys+1)))];
t2_keys=t_keys[which(is.element(t_keys, (t0_keys+2)))];
t0_keys_set=intersect(t0_keys, intersect(t1_keys-1, t2_keys-2));

tid_keys_sel=which(is.element(tid_keys, (t0_keys_set+0)));
tid_keys=tid_keys[tid_keys_sel];
tid=tid[tid_keys_sel,]
tid=tid[order(tid_keys),];

t0_keys_sel=which(is.element(t_keys, (t0_keys_set+0)));
t0_keys=t_keys[t0_keys_sel];
t0=t[t0_keys_sel,];
t0=t0[order(t0_keys),];

t1_keys_sel=which(is.element(t_keys, (t0_keys_set+1)));
t1_keys=t_keys[t1_keys_sel];
t1=t[t1_keys_sel,];
t1=t1[order(t1_keys),];

t2_keys_sel=which(is.element(t_keys, (t0_keys_set+2)));
t2_keys=t_keys[t2_keys_sel];
t2=t[t2_keys_sel,];
t2=t2[order(t2_keys),];
