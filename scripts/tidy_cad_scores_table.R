ca=commandArgs(TRUE);
t=read.table(ca[1], header=TRUE);

Target=t$target
Model=t$model
AA=(1-(t$sm0_AA_diff/t$sm0_AA_ref));
AS=(1-(t$sm0_AS_diff/t$sm0_AS_ref));
SA=(1-(t$sm0_SA_diff/t$sm0_SA_ref));
SS=(1-(t$sm0_SS_diff/t$sm0_SS_ref));
GDT_TS=t$TM_score_GDT_TS;
GDT_HA=t$TM_score_GDT_HA;
TM_score=t$TM_score

ot=data.frame(Target, Model, AA, AS, SA, SS, GDT_TS, GDT_HA, TM_score);
ot=format(ot, digits=3);
write.table(ot, ca[2], quote=FALSE, row.names=FALSE);
