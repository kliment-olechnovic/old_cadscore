#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to database directory
  -t    target name in the database
  -m    model name in the database
  -c    contacts category

EOF
}

SCRIPT_DIRECTORY=$(dirname $0)
VOROPROT="$SCRIPT_DIRECTORY/voroprot"

if [ ! -f "$VOROPROT" ]
then
  echo "$VOROPROT does not exist" 1>&2
  exit 1
fi

DATABASE=""
TARGET_NAME=""
MODEL_NAME=""
CATEGORY=""

while getopts "hD:t:m:c:" OPTION
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
      TARGET_NAME=$(basename "$OPTARG")
      ;;
    m)
      MODEL_NAME=$(basename "$OPTARG")
      ;;
    c)
      CATEGORY=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ] || [ -z "$TARGET_NAME" ] || [ -z "$MODEL_NAME" ] || [ -z "$CATEGORY" ]
then
  print_help
  exit 1
fi

COMBINED_INTER_RESIDUE_CONTACTS_FILE="$DATABASE/$TARGET_NAME/$MODEL_NAME/combined_inter_residue_contacts"

if [ ! -s "$COMBINED_INTER_RESIDUE_CONTACTS_FILE" ]
then
  echo "Combined inter-residue contacts file for target \"$TARGET_NAME\" and model \"$MODEL_NAME\" is not in the database" 1>&2
  exit 1
fi

cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE | $VOROPROT --mode print-combined-inter-residue-contacts-plot --category $CATEGORY
