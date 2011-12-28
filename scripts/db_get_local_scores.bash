#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

VOROPROT=$SCRIPT_DIRECTORY/voroprot2

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to database directory
  -t    target name in the database
  -m    model name in the database
  -c    contacts category
  -w    bluring window size

EOF
}

DATABASE=""
TARGET_NAME=""
MODEL_NAME=""
CATEGORY=""
WINDOW="0"

while getopts “hD:t:m:c:w:” OPTION
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
      TARGET_NAME=$OPTARG
      ;;
    m)
      MODEL_NAME=$OPTARG
      ;;
    c)
      CATEGORY=$OPTARG
      ;;
    w)
      WINDOW=$OPTARG
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

CAD_PROFILE_FILE="$DATABASE/cadscore/$TARGET_NAME/$MODEL_NAME/cad_profile"
if [ ! -f "$CAD_PROFILE_FILE" ]
then
  echo "Contacts area difference profile file for target \"$TARGET_NAME\" and model \"$MODEL_NAME\" is not in the database" 1>&2
  exit 1
fi

###########################################

cat $CAD_PROFILE_FILE | $VOROPROT --mode calculate-contact-area-difference-local-scores --category $CATEGORY --window $WINDOW
