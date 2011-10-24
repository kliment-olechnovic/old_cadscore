#!/bin/bash

cat $1 $2 | ./calc_score.bash 0 0 0 bounded
cat $1 $2 | ./calc_score.bash 0 1 0 simple
cat $1 $2 | ./calc_score.bash 0 1 1 late_bounded
cat $1 $2 | ./calc_score.bash 0 2 0 abag_totr_0
cat $1 $2 | ./calc_score.bash 1 2 0 abag_totr_1
