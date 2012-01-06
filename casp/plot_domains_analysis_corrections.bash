#!/bin/bash

mkdir -p ../results/plot_domains_analysis_corrections

R --vanilla --args \
../results/enhanced_table.txt \
../results/initial_inter_domains_table.txt \
../results/plot_domains_analysis_corrections/ \
< plot_domains_analysis_corrections.R
