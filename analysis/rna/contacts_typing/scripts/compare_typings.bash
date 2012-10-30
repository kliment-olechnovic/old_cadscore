#!/bin/bash

cat ./input/typed_contacts | sed 's/.pdb//' | awk '{print $1 "_" $2 "_" $3 $4 "_" $5 $6 "\t" $2 "\t" $7}' | sort > typed_contacts_ids_and_areas

( \
cat ./input/mc_annotate_stacking_interactions | sed 's/.pdb//' | sed 's/-\([[:upper:]]\)/ \1/g' | sed 's/\(.*\)\s\(.*\)\s\(.*\)\s:\s\(.*\)/\1_stack_\2_\3\tstack\t\4/' | tr ' ' '_' ; \
cat ./input/mc_annotate_base_pairing_interactions | sed 's/.pdb//' | sed 's/-\([[:upper:]]\)/ \1/g' | sed 's/\(.*\)\s\(.*\)\s\(.*\)\s:\s\(.*\)/\1_side_\2_\3\tside\t\4/' | tr ' ' '_' \
) | egrep '_stack_|_side_' | sort > mc_annotate_interactions_ids


R --vanilla < ./scripts/compare_typings.R

rm summary.png
montage histogram_*.png plot_*.png -geometry '100%' summary.png
