#!/bin/bash

R --vanilla < create_merged_table_from_raw_tables.R
./create_reformatted_table_from_merged_table.bash
R --vanilla < create_enhanced_table_from_reformatted_table.R

cat enhanced_table | column -t > beautiful_enhanced_table
mv beautiful_enhanced_table enhanced_table

rm merged_table
rm reformatted_table
