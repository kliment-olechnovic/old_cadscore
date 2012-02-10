#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -t    path to target file in PBD format
  -m    path to model file in PBD format

EOF
}

DATABASE=""
TARGET_FILE=""
MODEL_FILE=""

while getopts "hD:t:m:" OPTION
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
      TARGET_FILE=$OPTARG
      ;;
    m)
      MODEL_FILE=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ] || [ -z "$TARGET_FILE" ] || [ -z "$MODEL_FILE" ]
then
  print_help
  exit 1
fi

if [ -f "$TARGET_FILE" ]
then
  TARGET_NAME=$(basename $TARGET_FILE)
else
  echo "Target file \"$TARGET_FILE\" does not exist" 1>&2
  exit 1
fi

if [ -f "$MODEL_FILE" ]
then
  MODEL_NAME=$(basename $MODEL_FILE)
else
  echo "Model file \"$MODEL_FILE\" does not exist" 1>&2
  exit 1
fi

###########################################

$SCRIPT_DIRECTORY/db_set_atoms.bash -D $DATABASE -i $TARGET_FILE
$SCRIPT_DIRECTORY/db_set_atoms.bash -D $DATABASE -i $MODEL_FILE -t $TARGET_NAME
$SCRIPT_DIRECTORY/db_set_contacts.bash -D $DATABASE -n $TARGET_NAME
$SCRIPT_DIRECTORY/db_set_contacts.bash -D $DATABASE -n "$MODEL_NAME/$TARGET_NAME"
$SCRIPT_DIRECTORY/db_set_cadscore.bash -D $DATABASE -t $TARGET_NAME -m "$MODEL_NAME/$TARGET_NAME"
