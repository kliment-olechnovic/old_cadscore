#!/bin/bash

mkdir -p ../results/plot_refinement_targets

R --vanilla --args ../results/initial_refinement_table.txt ../results/plot_refinement_targets/ < plot_refinement_targets.R