#!/bin/bash

cat ./input/typed_contacts | sed 's/.pdb//' | awk '{print $1 "_" $2 "_" $3 $4 "_" $5 $6 " " $2 " " $7}' | sort > typed_contacts_ids_and_areas

( \
cat ./input/mc_annotate_stacking_interactions | sed 's/.pdb//' | sed 's/-\([[:upper:]]\)/ \1/g' | awk '{print $1 "_stack_" $2 "_" $3 " stack"}' ; \
cat ./input/mc_annotate_base_pairing_interactions | sed 's/.pdb//' | sed 's/-\([[:upper:]]\)/ \1/g' | awk '{print $1 "_side_" $2 "_" $3 " side"}' \
) | sort > mc_annotate_interactions_ids


R --vanilla < ./scripts/compare_typings.R

rm summary.png
montage histogram_*.png plot_*.png -geometry '100%' summary.png
