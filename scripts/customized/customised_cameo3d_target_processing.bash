#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 parameters:

  Required:
    -S    path to CAD-score scripts directory
    -D    path to writable database directory
    -I    path to input directory

  Other:
    -h    show this message and exit

EOF
}

SCRIPT_DIRECTORY=""
DATABASE=""
INPUT_DIRECTORY=""

while getopts "hS:D:I:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    S)
      SCRIPT_DIRECTORY=$OPTARG
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    I)
      INPUT_DIRECTORY=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$SCRIPT_DIRECTORY" ] || [ ! -d "$SCRIPT_DIRECTORY" ] || [ ! -s "$SCRIPT_DIRECTORY/CADscore_calc.bash" ]
then
  echo "Missing path to CAD-score scripts directory" 1>&2
  exit 1
fi

if [ -z "$DATABASE" ]
then
  echo "Missing path to database directory" 1>&2
  exit 1
fi

if [ -z "$INPUT_DIRECTORY" ] || [ ! -d "$INPUT_DIRECTORY" ]
then
  echo "Missing input directory" 1>&2
  exit 1
fi

TARGET_INPUT_FILE="$INPUT_DIRECTORY/target.pdb"
if [ ! -s "$TARGET_INPUT_FILE" ]
then
  echo "Missing input model file ($MODEL_INPUT_FILE)" 1>&2
  exit 1
fi

TMP_DIR=$(mktemp -d)

TARGET_NAME=$(basename $INPUT_DIRECTORY)

find $INPUT_DIRECTORY/servers -mindepth 1 -maxdepth 1 -type d | while read SERVER_DIR
do
  SERVER_ID=$(basename $SERVER_DIR)
  find $SERVER_DIR -mindepth 1 -maxdepth 1 -type d | while read MODEL_DIR
  do
    MODEL_ID=$(basename $MODEL_DIR)
    MODEL_INPUT_FILE="$MODEL_DIR/$MODEL_ID.pdb"
    if [ ! -s "$MODEL_INPUT_FILE" ]
    then
      echo "Missing input model file ($MODEL_INPUT_FILE)" 1>&2
      exit 1
    fi
    MODEL_NAME=$TARGET_NAME"_"$SERVER_ID"_"$MODEL_ID
    cp "$TARGET_INPUT_FILE" "$TMP_DIR/$TARGET_NAME"
    cp "$MODEL_INPUT_FILE" "$TMP_DIR/$MODEL_NAME"
    $SCRIPT_DIRECTORY/CADscore_calc.bash -D "$DATABASE" -t "$TMP_DIR/$TARGET_NAME" -m "$TMP_DIR/$MODEL_NAME" -r
  done
done

rm -r "$TMP_DIR"
