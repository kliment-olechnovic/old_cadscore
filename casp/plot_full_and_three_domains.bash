#!/bin/bash

mkdir -p ../results/plot_full_and_three_domains

R --vanilla --args ../results/enhanced_table.txt ../results/plot_full_and_three_domains/ < plot_full_and_three_domains.R