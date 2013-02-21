#!/bin/bash

NEED_TO_PRINT_HEADER=true

while read LOG_FILE
do
  if $NEED_TO_PRINT_HEADER
  then
    cat $LOG_FILE | cut --delimiter " " --fields 1 | paste -s
    NEED_TO_PRINT_HEADER=false
  fi
  cat $LOG_FILE | cut --delimiter " " --fields 2 | paste -s
done
