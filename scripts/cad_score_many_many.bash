#!/bin/bash

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
  -F    path to directory with filter files (optional)
  -f    filter file name pattern (optional, default is *)

EOF
}

TARGET_FILE_NAME_PATTERN="*"
MODEL_FILE_NAME_PATTERN="*"
FILTER_FILE_NAME_PATTERN="*"

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
    F)
      FILTERS_DIRECTORY=$OPTARG
      ;;
    f)
      FILTER_FILE_NAME_PATTERN=$OPTARG
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

if [ -n "$FILTERS_DIRECTORY" ] && [ ! -d "$FILTERS_DIRECTORY" ]
then
  echo "Filters directory \"$FILTERS_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

for TARGET_FILE in `find $TARGETS_DIRECTORY -name "$TARGET_FILE_NAME_PATTERN" -type f`
do
  for MODEL_FILE in `find $MODELS_DIRECTORY -name "$MODEL_FILE_NAME_PATTERN" -type f`
  do
    echo "Scoring target $TARGET_FILE and model $MODEL_FILE" 1>&2
    ./cad_score.bash -O $OUTPUT_DIRECTORY -t $TARGET_FILE -m $MODEL_FILE
    if [ -n "$FILTERS_DIRECTORY" ]
    then
      for FILTER_FILE in `find $FILTERS_DIRECTORY -name "$FILTER_FILE_NAME_PATTERN" -type f`
      do
        echo "  Using filter $FILTER_FILE" 1>&2
        ./cad_score.bash -O $OUTPUT_DIRECTORY -t $TARGET_FILE -m $MODEL_FILE -f $FILTER_FILE
      done
    fi
  done
done
