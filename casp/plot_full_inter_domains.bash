#!/bin/bash

R --vanilla --args \
../results/enhanced_table.txt \
../results/initial_inter_domains_table.txt \
../results/inter_domains_insiding_vs_orientation.png \
../results/inter_domains_diff_from_worst_vs_orientation.png \
../results/inter_domains_scaled_diff_vs_orientation.png \
../results/inter_domains_orientation_vs_scores.png \
< plot_full_inter_domains.R
