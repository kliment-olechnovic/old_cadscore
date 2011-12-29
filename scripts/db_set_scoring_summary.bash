#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to existing writable database directory
  
EOF
}

DATABASE=""

while getopts “hD:” OPTION
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

###########################################

OUTPUT_DIRECTORY="$DATABASE/scoring_summary"
mkdir -p $OUTPUT_DIRECTORY

TABLE_FILE=$OUTPUT_DIRECTORY/table
$SCRIPT_DIRECTORY/util_cat_tables.bash -S $DATABASE -p "*scoring_report/*/table_row" -f "target" > $TABLE_FILE

SHORT_TABLE_FILE=$OUTPUT_DIRECTORY/short_table
awk '{print $1 " " $2 " " $9 " " $15 " " $33 " " $39 " " $44 " " $45}' $TABLE_FILE > $SHORT_TABLE_FILE
