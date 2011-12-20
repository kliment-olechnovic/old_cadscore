#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -O    path to writable output directory
  -T    path to directory with target files
  -t    target file name pattern (optional, default is *)
  -M    path to directory with model files
  -m    model file name pattern (optional, default is *)

EOF
}

OUTPUT_DIRECTORY=""
TARGETS_DIRECTORY=""
TARGET_FILE_NAME_PATTERN="*"
MODELS_DIRECTORY=""
MODEL_FILE_NAME_PATTERN="*"

while getopts “hO:T:t:M:m:F:f:” OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    O)
      OUTPUT_DIRECTORY=$OPTARG
      ;;
    T)
      TARGETS_DIRECTORY=$OPTARG
      ;;
    t)
      TARGET_FILE_NAME_PATTERN=$OPTARG
      ;;
    M)
      MODELS_DIRECTORY=$OPTARG
      ;;
    m)
      MODEL_FILE_NAME_PATTERN=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$OUTPUT_DIRECTORY" ] || [ -z "$TARGETS_DIRECTORY" ] || [ -z "$MODELS_DIRECTORY" ]
then
  print_help
  exit 1
fi

if [ ! -d "$TARGETS_DIRECTORY" ]
then
  echo "Target directory \"$TARGETS_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

if [ ! -d "$MODELS_DIRECTORY" ]
then
  echo "Models directory \"$MODELS_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

###########################################

for TARGET_FILE in `find $TARGETS_DIRECTORY -name "$TARGET_FILE_NAME_PATTERN" -type f`
do
  for MODEL_FILE in `find $MODELS_DIRECTORY -name "$MODEL_FILE_NAME_PATTERN" -type f`
  do
    echo "Scoring target $TARGET_FILE and model $MODEL_FILE" 1>&2
    $SCRIPT_DIRECTORY/db_set_scoring_report.bash -O $OUTPUT_DIRECTORY -t $TARGET_FILE -m $MODEL_FILE
  done
done