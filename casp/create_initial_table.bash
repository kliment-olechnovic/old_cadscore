#!/bin/bash

TEMP_FILE_1="temp_file_1.txt"
TEMP_FILE_2="temp_file_2.txt"

./cat_two_tables.bash ../data/domains.txt ../data/full.txt > $TEMP_FILE_1
R --vanilla --args $TEMP_FILE_1 casp_8_9_lga_sda.txt $TEMP_FILE_2 < merge_tables.R
./format_key_columns.bash $TEMP_FILE_2 > ../results/initial_table.txt
rm $TEMP_FILE_1 $TEMP_FILE_2
