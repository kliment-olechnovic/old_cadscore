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

score_name="sm0_AA"
normalised_orientation_error=tid[,paste(score_name, "_diff", sep="")]/t0[,paste(score_name, "_ref", sep="")];

score_insiding_value=abs(t1[, score_name]-t2[, score_name])-(abs(t0[, score_name]-t1[, score_name])+abs(t0[, score_name]-t2[, score_name]));
GDT_TS_insiding_value=(abs(t1$GDT_TS-t2$GDT_TS)-(abs(t0$GDT_TS-t1$GDT_TS)+abs(t0$GDT_TS-t2$GDT_TS)))/100;
png(cmd_args[3], width=12, height=7, units="in", res=200);
plot(x=GDT_TS_insiding_value, y=normalised_orientation_error, col="red", cex=0.5, xlab="Insiding", ylab="Normalized orientation error", main="Insiding vs orientation error");
points(x=score_insiding_value, y=normalised_orientation_error, col="blue", cex=0.5);
dev.off();

score_lower_value=t0[, score_name]-pmin(t1[, score_name], t2[, score_name]);
GDT_TS_lower_value=(t0$GDT_TS-pmin(t1$GDT_TS, t2$GDT_TS))/100;
png(cmd_args[4], width=12, height=7, units="in", res=200);
plot(x=GDT_TS_lower_value, y=normalised_orientation_error, col="red", cex=0.5, xlab="Difference from worst", ylab="Normalized orientation error", main="Difference from worst vs orientation error");
points(x=score_lower_value, y=normalised_orientation_error, col="blue", cex=0.5);
points(x=c(0,0), y=c(0,1), type="l", col="black");
dev.off();
