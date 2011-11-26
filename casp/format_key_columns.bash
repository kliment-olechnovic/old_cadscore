#!/bin/bash

INPUT_TABLE=$1

#Example:
#model target
#T0388TS002_1-D1.lga T0388-D1
#T0387TS002_1.lga T0387

cat $INPUT_TABLE | \
sed 's/T0\(...\)TS\(...\)_1-D\(.\)\.lga\sT0...-D./\1 \3 \2/' | \
sed 's/T0\(...\)TS\(...\)_1\.lga\sT0.../\1 0 \2/' | \
sed 's/model target/target domain group/'
