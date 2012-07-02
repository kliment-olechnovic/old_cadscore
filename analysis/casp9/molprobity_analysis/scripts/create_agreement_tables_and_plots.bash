R --vanilla < scripts/plot_molprobity_distribution_analysis.R

R --vanilla --args "0" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_0" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "1" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_1" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sd" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sd" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sdall" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sdall" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sdgdt" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sdgdt" < scripts/plot_molprobity_judging_analysis.R

R --vanilla --args "sdcad" < scripts/create_agreement_table.R
R --vanilla --args "judging_from_sdcad" < scripts/plot_molprobity_judging_analysis.R
