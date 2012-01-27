#!/bin/bash

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -m    path to file in PBD format

EOF
}

DATABASE=""
MODEL_FILE=""

while getopts "hD:m:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    m)
      MODEL_FILE=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ] || [ -z "$MODEL_FILE" ]
then
  print_help
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

OUTPUT_DIRECTORY="$DATABASE/molprobity/$MODEL_NAME"
mkdir -p $OUTPUT_DIRECTORY

MOLPROBITY_PROFILE_FILE="$OUTPUT_DIRECTORY/molprobity_profile"
test -f $MOLPROBITY_PROFILE_FILE || molprobity.bash $MODEL_FILE > $MOLPROBITY_PROFILE_FILE 2> "$MOLPROBITY_PROFILE_FILE.log"

MOLPROBITY_SCORE=$(cat $MOLPROBITY_PROFILE_FILE | cut -d":" -f28 | tail -n 1)
if [ -z "$MOLPROBITY_SCORE" ] ; then MOLPROBITY_SCORE="-1" ; fi

MOLPROBITY_SUMMARY_FILE="$OUTPUT_DIRECTORY/molprobity_summary"
echo "MolProbityScore $MOLPROBITY_SCORE" > $MOLPROBITY_SUMMARY_FILE
