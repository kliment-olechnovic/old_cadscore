#!/bin/bash

SEARCH_DIRECTORY=$1
PATH_PATTERN=$2

HEADER=$(cat `find $SEARCH_DIRECTORY -path $PATH_PATTERN -type f | head -1` | head -1)
echo $HEADER

for F in `find $SEARCH_DIRECTORY -path $PATH_PATTERN -type f`
do
  cat $F | sed -n '2p'
done
