#!/bin/bash

INPUT=$1

echo "pdb_id contact_id first_base_name second_base_name contact_type contact_area"
cat $INPUT | egrep 'left_to_right' | sed 's/.pdb//' | sed 's/na_stacking/stack/' | sed 's/na_siding/side/' | awk '{print $1 " " $1 "_" $2 $3 "-" $5 $6 " " $4 " " $7 " " $8 " " $9}'
