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

while getopts “hD:t:m:” OPTION
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

OUTPUT_DIRECTORY="$DATABASE/tmscore/$TARGET_NAME/$MODEL_NAME"
mkdir -p $OUTPUT_DIRECTORY

TMSCORE_PROFILE_FILE="$OUTPUT_DIRECTORY/tmscore_profile"
test -f $TMSCORE_PROFILE_FILE || TMscore $MODEL_FILE $TARGET_FILE > $TMSCORE_PROFILE_FILE 2> "$TMSCORE_PROFILE_FILE.log"

TM_SCORE=$(cat $TMSCORE_PROFILE_FILE | egrep "TM-score\s*=.*d0" | sed 's/TM-score\s*=\s*\(.*\)\s*(.*/\1/g')
if [ -z "$TM_SCORE" ] ; then TM_SCORE=0 ; fi

TM_SCORE_GDT_TS=$(cat $TMSCORE_PROFILE_FILE | egrep "GDT-TS-score" | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g')
if [ -z "$TM_SCORE_GDT_TS" ] ; then TM_SCORE_GDT_TS=0 ; fi

TM_SCORE_GDT_HA=$(cat $TMSCORE_PROFILE_FILE | egrep "GDT-HA-score" | sed 's/GDT-HA-score\s*=\s*\(.*\)\s*%(d<0\.5).*/\1/g')
if [ -z "$TM_SCORE_GDT_HA" ] ; then TM_SCORE_GDT_HA=0 ; fi

TMSCORE_SUMMARY_FILE="$OUTPUT_DIRECTORY/tmscore_summary"
echo "TM_score $TM_SCORE" > $TMSCORE_SUMMARY_FILE
echo "TM_score_GDT_TS $TM_SCORE_GDT_TS" >> $TMSCORE_SUMMARY_FILE
echo "TM_score_GDT_HA $TM_SCORE_GDT_HA" >> $TMSCORE_SUMMARY_FILE
