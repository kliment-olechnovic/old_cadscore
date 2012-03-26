#!/bin/bash

R --vanilla < scripts/create_merged_table_from_raw_tables.R

cat merged_table | sed 's/T0\(...\)TS\(...\)_1.*\.lga\sT0...-D\(.\)/\1 \3 \2/' | sed 's/model target/target domain group/' > reformatted_table

R --vanilla < scripts/create_enhanced_table_from_reformatted_table.R

cat enhanced_table | column -t > beautiful_enhanced_table
mv beautiful_enhanced_table enhanced_table

rm merged_table
rm reformatted_table
