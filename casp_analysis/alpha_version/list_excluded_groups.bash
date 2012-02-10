#!/bin/bash

mkdir -p ../results/list_excluded_groups

R --vanilla --args \
../results/enhanced_table.txt \
../results/list_excluded_groups/ \
< list_excluded_groups.R
