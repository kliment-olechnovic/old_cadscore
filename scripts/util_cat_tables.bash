#!/bin/bash

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -S    path to search start directory
  -p    files path pattern
  -f    filtering pattern

EOF
}

while getopts "hS:p:f:" OPTION
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
    f)
      FILTER=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$SEARCH_DIRECTORY" ] || [ -z "$PATH_PATTERN" ] || [ -z "$FILTER" ]
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
    echo "$F is not a valid part of a table" 1>&2
  fi
done
