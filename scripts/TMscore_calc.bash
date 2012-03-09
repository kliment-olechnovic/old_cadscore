#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -t    path to target file in PBD format
  -m    path to model file in PBD format
  -p    profile output file
  -s    summary output file

EOF
}

TARGET_FILE=""
MODEL_FILE=""
TMSCORE_PROFILE_FILE=""
TMSCORE_SUMMARY_FILE=""

while getopts "ht:m:p:s:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    t)
      TARGET_FILE=$OPTARG
      ;;
    m)
      MODEL_FILE=$OPTARG
      ;;
    p)
      TMSCORE_PROFILE_FILE=$OPTARG
      ;;
    s)
      TMSCORE_SUMMARY_FILE=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$TARGET_FILE" ] || [ -z "$MODEL_FILE" ] || [ -z "$TMSCORE_PROFILE_FILE" ] || [ -z "$TMSCORE_SUMMARY_FILE" ]
then
  print_help
  exit 1
fi

if [ ! -f "$TARGET_FILE" ]
then
  echo "Target file \"$TARGET_FILE\" does not exist" 1>&2
  exit 1
fi

if [ ! -f "$MODEL_FILE" ]
then
  echo "Model file \"$MODEL_FILE\" does not exist" 1>&2
  exit 1
fi

TEMP_SHORT_TARGET_FILE=$(mktemp)
TEMP_SHORT_MODEL_FILE=$(mktemp)

cp $TARGET_FILE $TEMP_SHORT_TARGET_FILE
cp $MODEL_FILE $TEMP_SHORT_MODEL_FILE

TMscore $TEMP_SHORT_MODEL_FILE $TEMP_SHORT_TARGET_FILE > $TMSCORE_PROFILE_FILE
rm $TEMP_SHORT_MODEL_FILE $TEMP_SHORT_TARGET_FILE

TM_SCORE=$(cat $TMSCORE_PROFILE_FILE | egrep "TM-score\s*=.*d0" | sed 's/TM-score\s*=\s*\(.*\)\s*(.*/\1/g')
if [ -z "$TM_SCORE" ] ; then TM_SCORE=0 ; fi

TM_SCORE_GDT_TS=$(cat $TMSCORE_PROFILE_FILE | egrep "GDT-TS-score" | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g')
if [ -z "$TM_SCORE_GDT_TS" ] ; then TM_SCORE_GDT_TS=0 ; fi

TM_SCORE_GDT_HA=$(cat $TMSCORE_PROFILE_FILE | egrep "GDT-HA-score" | sed 's/GDT-HA-score\s*=\s*\(.*\)\s*%(d<0\.5).*/\1/g')
if [ -z "$TM_SCORE_GDT_HA" ] ; then TM_SCORE_GDT_HA=0 ; fi

echo "TM_score $TM_SCORE" > $TMSCORE_SUMMARY_FILE
echo "TM_score_GDT_TS $TM_SCORE_GDT_TS" >> $TMSCORE_SUMMARY_FILE
echo "TM_score_GDT_HA $TM_SCORE_GDT_HA" >> $TMSCORE_SUMMARY_FILE
