args=commandArgs(trailingOnly=TRUE);
working_set=args[1];

cadscore_variants=c("AA", "AS", "SS", "MM");

for(cadscore_variant in cadscore_variants)
{

rt=read.table(paste("./output/", working_set, "/molprobity_results_", cadscore_variant, ".txt", sep=""), header=TRUE, stringsAsFactors=FALSE);

second_score_names=union(rt$second_score_name, rt$second_score_name);

for(second_score_name in second_score_names)
{
	t=rt[which(rt$second_score_name==second_score_name),];
	max_RMSD_values=rev(sort(union(t$max_RMSD, t$max_RMSD)));
	par(mar=c(12, 5, 5, 4), xpd=TRUE);
	plot(x=c(0, 1), y=c(0, 1), xlim=c(0, 1), ylim=c(0, 1), type="n", xlab="Minimum RNA_INF in a pair", ylab="Fraction", main=paste("Fractions for CAD-score ", cadscore_variant, " and ", second_score_name, sep=""));
	for(max_RMSD_value in max_RMSD_values)
	{
		intensity=((which(max_RMSD_values==max_RMSD_value)-1)/(length(max_RMSD_values)*1.2));
		red_color=rgb(1, intensity, intensity, maxColorValue=1)[1];
		green_color=rgb(intensity, 1, intensity, maxColorValue=1)[1];
		blue_color=rgb(intensity, intensity, 1, maxColorValue=1)[1];
		black_color=rgb(intensity, intensity, intensity, maxColorValue=1)[1];
		magenta_color=rgb(1, intensity, 1, maxColorValue=1)[1];
		ft=t[which(t$max_RMSD==max_RMSD_value),];
		subset_names=sort(union(ft$subset_name, ft$subset_name));
		s1=ft[which(ft$subset_name==subset_names[1]),];
		s2=ft[which(ft$subset_name==subset_names[2]),];
		s3=ft[which(ft$subset_name==subset_names[3]),];
		s4=ft[which(ft$subset_name==subset_names[4]),];
		points(x=s1$min_RNA_INF, y=s1$subset_size/s1$set_size, type="l", lwd=1.0, col=blue_color);
		points(x=s1$min_RNA_INF, y=s2$subset_size/(s2$subset_size+s3$subset_size), type="l", lwd=1.0, col=green_color);
		points(x=s1$min_RNA_INF, y=s3$subset_size/(s2$subset_size+s3$subset_size), type="l", lwd=1.0, col=red_color);
		points(x=s1$min_RNA_INF, y=s4$subset_size/s1$set_size, type="l", lwd=1.0, col=magenta_color);
		points(x=s1$min_RNA_INF, y=s1$set_size/max(s1$set_size), type="l", lwd=1.0, col=black_color);
	}
	legend(0, -0.35, c("Filtered by minimum RNA_INF in a pair", "Both agreed with MP", "CAD-score agreed with MP", "CAD-score disagreed with MP", "Both disagreed with MP"), lwd=c(1, 1, 1, 1, 1), col=c("black", "blue", "green", "red", "magenta"));
}

}
