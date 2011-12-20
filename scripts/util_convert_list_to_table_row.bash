#!/bin/bash

###########################################

LIST_FILE=$1

###########################################

for i in 1 2 ; do cat $LIST_FILE | cut --delimiter " " --fields $i | paste -s ; done
