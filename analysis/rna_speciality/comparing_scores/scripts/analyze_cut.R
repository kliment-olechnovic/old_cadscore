input_table_file="../collecting_scores/output/cut/calculated_scores";

t=read.table(input_table_file, header=TRUE, stringsAsFactors=FALSE);

fraction_of_atoms=t$m_atoms/t$t_atoms;
t=t[order(fraction_of_atoms),];
fraction_of_atoms=t$m_atoms/t$t_atoms;

png("plot_of_scores_of_cut.png", height=4.8, width=5.0, units="in", res=300);

plot(fraction_of_atoms, fraction_of_atoms, type="l", col="darkcyan", lwd=2, xlim=c(0, 1), ylim=c(0, 1), main="Model completeness and scores", xlab="Fraction of atoms", ylab="Scores");
points(fraction_of_atoms, fraction_of_atoms^0.5, type="l", col="darkorange", lwd=2);
points(fraction_of_atoms, t$rna_inf_norv, col="red", pch=1, cex=0.5);
points(fraction_of_atoms, t$SS, col="blue", pch=1, cex=0.5);
legend("bottomright", c("y=x curve", "y=sqrt(x) curve", "CAD-score SS", "RNA INF score"), lty=c("solid", "solid", NA, NA), lwd=c(2, 2, NA, NA), pch=c(NA, NA, 1, 1), col=c("darkcyan", "darkorange", "blue", "red"));

dev.off();
