#!/bin/bash

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to existing database directory
  -t    flag to print table
  -l    flag to print logs
  -p    flag to make output pretty
  
EOF
}

DATABASE=""
FLAG_TABLE=false
FLAG_LOGS=false
FLAG_PRETTY=false

while getopts "hD:tlp" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    t)
      FLAG_TABLE=true
      ;;
    l)
      FLAG_LOGS=true
      ;;
    p)
      FLAG_PRETTY=true
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

###########################################

if $FLAG_TABLE
then
  if $FLAG_PRETTY
  then
  	cat $DATABASE/scoring_summary/table | column -t
  else
    cat $DATABASE/scoring_summary/table
  fi
fi

if $FLAG_LOGS
then
  cat $DATABASE/scoring_summary/logs
fi
