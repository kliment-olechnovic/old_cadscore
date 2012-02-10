#!/bin/bash

TEMP_FILE="temp_file.txt"

R --vanilla --args ../data/full_inter_domains.txt casp_8_9_lga_sda.txt $TEMP_FILE < merge_tables.R
./format_key_columns.bash $TEMP_FILE > ../results/initial_inter_domains_table.txt
rm $TEMP_FILE
