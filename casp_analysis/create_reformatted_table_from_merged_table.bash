#!/bin/bash

cat merged_table | sed 's/T0\(...\)TS\(...\)_1.*\.lga\sT0...-D\(.\)/\1 \3 \2/' | sed 's/model target/target domain group/' > reformatted_table
