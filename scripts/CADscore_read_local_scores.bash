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
  -w    bluring window size (optional)
  -s    flag to simply print local scores (optional)
  -i    flag to print local scores plot (optional)
  -p    path to file in the PDB format to inject in (optional)

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
WINDOW="0"
SIMPLE_PRINTING=false
IMAGE_PRINTING=false
PDB_FILE=""

while getopts "hD:t:m:c:w:sip:" OPTION
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
    w)
      WINDOW=$OPTARG
      ;;
    s)
      SIMPLE_PRINTING=true
      ;;
    i)
      IMAGE_PRINTING=true
      ;;
    p)
      PDB_FILE=$OPTARG
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

CAD_PROFILE_FILE="$DATABASE/$TARGET_NAME/$MODEL_NAME/cad_profile"

if [ ! -s "$CAD_PROFILE_FILE" ]
then
  echo "Contacts area difference profile file for target \"$TARGET_NAME\" and model \"$MODEL_NAME\" is not in the database" 1>&2
  exit 1
fi

if $SIMPLE_PRINTING
then
  cat $CAD_PROFILE_FILE | $VOROPROT --mode calc-CAD-local-scores --category $CATEGORY --window $WINDOW
fi

if $IMAGE_PRINTING
then
  cat $CAD_PROFILE_FILE | $VOROPROT --mode print-CAD-local-scores-plot --category $CATEGORY --max-window $WINDOW
fi

if [ -n "$PDB_FILE" ]
then
  (cat $CAD_PROFILE_FILE | $VOROPROT --mode calc-CAD-local-scores --category $CATEGORY --window $WINDOW ; cat $PDB_FILE) | $VOROPROT --mode print-CAD-local-scores-to-PDB-file
fi
