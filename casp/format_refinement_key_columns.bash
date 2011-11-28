#!/bin/bash

INPUT_TABLE=$1

#Example:
#target model
#T0567-TR.pdb TR567
#T0567-TR.pdb TR567TS484_5

cat $INPUT_TABLE | \
sed 's/T0\(...\)-TR.pdb\sTR...TS\(...\)_\(.\)/\1 \2 \3/' | \
sed 's/T0\(...\)-TR.pdb\sTR.../\1 0 0/' | \
sed 's/target model/target group model/'
