#!/bin/bash

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

OUTPUT_DIRECTORY="$DATABASE/molprobity/$TARGET_NAME/$MODEL_NAME"
mkdir -p $OUTPUT_DIRECTORY

TMSCORE_PROFILE_FILE="$OUTPUT_DIRECTORY/tmscore_profile"
test -f $TMSCORE_PROFILE_FILE || TMscore $MODEL_FILE $TARGET_FILE > $TMSCORE_PROFILE_FILE 2> "$TMSCORE_PROFILE_FILE.log"
TM_SCORE_GDT_TS=$(cat $TMSCORE_PROFILE_FILE | egrep "GDT-TS-score" | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g')
if [ -z "$TM_SCORE_GDT_TS" ] ; then TM_SCORE_GDT_TS=0 ; fi

CONDITION=$(echo "$TM_SCORE_GDT_TS > 0.6" | bc -l);

MOLPROBITY_SCORE="-1"
if [ "$CONDITION" -eq "1" ]
then
  MOLPROBITY_PROFILE_FILE="$OUTPUT_DIRECTORY/molprobity_profile"
  test -f $MOLPROBITY_PROFILE_FILE || molprobity.bash $MODEL_FILE > $MOLPROBITY_PROFILE_FILE 2> "$MOLPROBITY_PROFILE_FILE.log"
  MOLPROBITY_SCORE=$(cat $MOLPROBITY_PROFILE_FILE | cut -d":" -f28 | tail -n 1)
  if [ -z "$MOLPROBITY_SCORE" ] ; then MOLPROBITY_SCORE="-1" ; fi
fi

MOLPROBITY_SUMMARY_FILE="$OUTPUT_DIRECTORY/molprobity_summary"
echo "MolProbityScore $MOLPROBITY_SCORE" > $MOLPROBITY_SUMMARY_FILE
