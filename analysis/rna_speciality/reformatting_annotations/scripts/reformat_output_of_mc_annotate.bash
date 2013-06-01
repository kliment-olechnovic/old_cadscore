#!/bin/bash

INPUT=$1

echo "pdb_id contact_id prop1 prop2 prop3 prop4 prop5 prop6"
cat $INPUT | egrep -v " '" | sed 's/.pdb//' | awk '{print $1 " " $1 "_" $2 " P_" $4 " P_" $5 " P_" $6 " P_" $7 " P_" $8 " P_" $9}'
