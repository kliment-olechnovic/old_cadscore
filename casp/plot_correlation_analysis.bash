#!/bin/bash

mkdir -p ../results/plot_correlation_analysis

R --vanilla --args \
../results/enhanced_table.txt \
../results/plot_correlation_analysis/ \
< plot_correlation_analysis.R
