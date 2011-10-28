#!/bin/bash

TEMP_FILE="/tmp/$(basename $0).$RANDOM.txt"

TMscore $1 $2 > $TEMP_FILE

./parse_TMScore_output.bash $TEMP_FILE
