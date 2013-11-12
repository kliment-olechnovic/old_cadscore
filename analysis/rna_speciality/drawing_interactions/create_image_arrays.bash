#!/bin/bash

./prepare_parameters.bash ~/rna_xray ../comparing_annotations/missed_mc_annotate_siding_interactions ./missed_siding_interactions_images > missed_siding_interactions_parameters
./prepare_parameters.bash ~/rna_xray ../comparing_annotations/differences_from_mc_annotate ./differences_from_mc_annotate_images > differences_from_mc_annotate_parameters

cat missed_siding_interactions_parameters | xargs -L 1 -P 1 ./draw_interaction.bash
cat differences_from_mc_annotate_parameters | head -500 | xargs -L 1 -P 1 ./draw_interaction.bash

./montage_images.bash ./missed_siding_interactions_images/ ./missed_siding_interactions_montage.jpg
./montage_images.bash ./differences_from_mc_annotate_images/ ./differences_from_mc_annotate_montage.jpg
