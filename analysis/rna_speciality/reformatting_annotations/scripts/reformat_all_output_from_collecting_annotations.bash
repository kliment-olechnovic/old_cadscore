#!/bin/bash

mkdir -p output

./scripts/reformat_output_of_voroprot2.bash ../collecting_annotations/output/stacking_and_siding_by_voroprot2_contacts | column -t > ./output/stack_and_side_by_voroprot2_contacts
./scripts/reformat_output_of_voroprot2.bash ../collecting_annotations/output/stacking_and_siding_by_voroprot2_faces | column -t > ./output/stack_and_side_by_voroprot2_faces
./scripts/reformat_output_of_mc_annotate.bash ../collecting_annotations/output/stacking_by_mc_annotate | column -t > ./output/stack_by_mc_annotate
./scripts/reformat_output_of_mc_annotate.bash ../collecting_annotations/output/base_pairing_by_mc_annotate | column -t > ./output/side_by_mc_annotate
