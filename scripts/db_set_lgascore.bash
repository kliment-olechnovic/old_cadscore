#!/bin/bash

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -t    target file
  -m    model file

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

OUTPUT_DIRECTORY="$DATABASE/lgascore/$TARGET_NAME/$MODEL_NAME"
mkdir -p $OUTPUT_DIRECTORY

LGA_PROFILE_FILE="$OUTPUT_DIRECTORY/lgascore_profile"
test -f $LGA_PROFILE_FILE || lga.bash $MODEL_FILE $TARGET_FILE > $LGA_PROFILE_FILE 2> "$LGA_PROFILE_FILE.log"

LGA_GDT_TS=$(cat $LGA_PROFILE_FILE | egrep '^SUMMARY' | awk '{print $7}')
if [ -z "$LGA_GDT_TS" ] ; then LGA_GDT_TS=0 ; fi

LGA_SUMMARY_FILE="$OUTPUT_DIRECTORY/lgascore_summary"
echo "LGA_GDT_TS $LGA_GDT_TS" > $LGA_SUMMARY_FILE
