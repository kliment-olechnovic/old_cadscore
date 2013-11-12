#!/bin/bash

INPUTDIR=$1
OUTPUT=$2

montage -label '%f' $INPUTDIR/*.png -tile 10x $OUTPUT
