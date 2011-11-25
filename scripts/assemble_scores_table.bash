#!/bin/bash

SEARCH_DIRECTORY=$1
PATH_PATTERN=$2
FILTER=$3

HEADER=""

for F in `find $SEARCH_DIRECTORY -path $PATH_PATTERN -type f`
do
  if grep --quiet $FILTER $F
  then
    if [ -z "$HEADER" ]
    then
      HEADER=$(cat $F | head -1)
      echo $HEADER
    fi
    cat $F | tail -n +2
  else
    echo "$F is bad" 1>&2
  fi
done
