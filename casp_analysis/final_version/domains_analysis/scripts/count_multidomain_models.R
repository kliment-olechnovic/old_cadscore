t=read.table("../prepared_data/enhanced_table", header=TRUE, stringsAsFactors=FALSE);

names=c();
numbers=c();

second_domains_set=t$target[which(t$domain==2)];
targets_set=union(second_domains_set, second_domains_set);

names=c(names, "multidomain_targets");
numbers=c(numbers, length(targets_set));

names=c(names, "multidomain_models");
numbers=c(numbers, length(second_domains_set));

result=data.frame(names=names, numbers=numbers);

write.table(result, "multidomain_models_counts", quote=FALSE, row.names=FALSE, col.names=FALSE);
