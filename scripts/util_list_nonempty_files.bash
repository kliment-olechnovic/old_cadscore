#!/bin/bash

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -S    path to search start directory
  -p    files path pattern

EOF
}

while getopts "hS:p:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    S)
      SEARCH_DIRECTORY=$OPTARG
      ;;
    p)
      PATH_PATTERN=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$SEARCH_DIRECTORY" ] || [ -z "$PATH_PATTERN" ]
then
  print_help
  exit 1
fi

if [ ! -d "$SEARCH_DIRECTORY" ]
then
  echo "Search directory \"$SEARCH_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

###########################################

for F in `find $SEARCH_DIRECTORY -path "$PATH_PATTERN" -type f ! -size 0`
do
  echo $F
done
