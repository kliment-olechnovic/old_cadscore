#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to existing database directory
  
EOF
}

DATABASE=""

while getopts "hD:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ]
then
  print_help
  exit 1
fi

if [ ! -d "$DATABASE" ]
then
  echo "Database directory \"$DATABASE\" does not exist" 1>&2
  exit 1
fi

NEED_TO_PRINT_HEADER=true

find "$DATABASE" -name "summary" -type f | while read SUMMARY_FILE
do
  if $NEED_TO_PRINT_HEADER
  then
    cat $SUMMARY_FILE | cut --delimiter " " --fields 1 | paste -s
    NEED_TO_PRINT_HEADER=false
  fi
  cat $SUMMARY_FILE | cut --delimiter " " --fields 2 | paste -s
done
