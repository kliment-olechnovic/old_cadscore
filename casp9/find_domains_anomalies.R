t=read.table("enhanced_table", header=TRUE, stringsAsFactors=FALSE);
i=read.table("casp9_initerfaces_table", header=TRUE, stringsAsFactors=FALSE);

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
