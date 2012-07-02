R --vanilla < scripts/plot_molprobity_distribution_analysis.R

R --vanilla --args "0" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_0" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "1" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_1" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sd" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sd" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sd_or" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sd_or" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sd_and" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sd_and" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sd_gdt" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sd_gdt" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sd_cad" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sd_cad" < scripts/plot_molprobity_judging_analysis.R
