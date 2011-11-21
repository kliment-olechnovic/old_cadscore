ca=commandArgs(TRUE);
t=read.table(ca[1], header=TRUE);

AA=(1-(t$sm0_AA_diff/t$sm0_AA_ref));
SA=(1-(t$sm0_SA_diff/t$sm0_SA_ref));
SS=(1-(t$sm0_SS_diff/t$sm0_SS_ref));
GDT_TS=t$TM_score_GDT_TS;
GDT_HA=t$TM_score_GDT_HA;
TM_score=t$TM_score

ot=data.frame(AA, SA, SS, GDT_TS, GDT_HA, TM_score);

write.table(ot, ca[2], quote=FALSE, row.names=FALSE);
