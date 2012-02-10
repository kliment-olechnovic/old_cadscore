#!/bin/bash

for t in `find ../data/full/ -mindepth 1 -maxdepth 1 -type d`
do
  tb=$(basename $t)
  list_of_rids=""
  for rids in `find ../data/domains/$tb* -path "*$tb-D*/output/cad_score/contacts/$tb-D*residue_ids" -type f`
  do
    list_of_rids=$list_of_rids" "$rids
  done
  if [[ $list_of_rids == *D2* ]]
  then
    echo "mkdir -p $t/filter/"
    echo "cat $list_of_rids > $t/filter/domains_residue_ids"
  fi
done
