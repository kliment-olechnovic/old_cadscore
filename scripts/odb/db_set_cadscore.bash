#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

VOROPROT=$SCRIPT_DIRECTORY/voroprot2

SCORING_MODE="0"

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -t    target name in the database
  -m    model name in the database

EOF
}

DATABASE=""
TARGET_NAME=""
MODEL_NAME=""

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
      TARGET_NAME=$OPTARG
      ;;
    m)
      MODEL_NAME=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ] || [ -z "$TARGET_NAME" ] || [ -z "$MODEL_NAME" ]
then
  print_help
  exit 1
fi

TARGET_CONTACTS_FILE="$DATABASE/contacts/$TARGET_NAME/inter_atom_contacts"
if [ ! -f "$TARGET_CONTACTS_FILE" ]
then
  echo "Contacts file for target \"$TARGET_NAME\" is not in the database" 1>&2
  exit 1
fi

MODEL_CONTACTS_FILE="$DATABASE/contacts/$MODEL_NAME/inter_atom_contacts"
if [ ! -f "$MODEL_CONTACTS_FILE" ]
then
  echo "Contacts file for model \"$MODEL_NAME\" is not in the database" 1>&2
  exit 1
fi

TARGET_RESIDUE_IDS_FILE="$DATABASE/atoms/$TARGET_NAME/residue_ids"
if [ ! -f "$TARGET_RESIDUE_IDS_FILE" ]
then
  echo "Residue IDs file for target \"$TARGET_NAME\" is not in the database" 1>&2
  exit 1
fi

###########################################

OUTPUT_DIRECTORY="$DATABASE/cadscore/$TARGET_NAME/$MODEL_NAME"
mkdir -p $OUTPUT_DIRECTORY

COMBINED_INTER_RESIDUE_CONTACTS_FILE="$OUTPUT_DIRECTORY/combined_inter_residue_contacts"
test -f $COMBINED_INTER_RESIDUE_CONTACTS_FILE || cat $TARGET_CONTACTS_FILE $MODEL_CONTACTS_FILE | $VOROPROT --mode combine-inter-residue-contacts > $COMBINED_INTER_RESIDUE_CONTACTS_FILE 2> "$COMBINED_INTER_RESIDUE_CONTACTS_FILE.log"

CAD_PROFILE_FILE="$OUTPUT_DIRECTORY/cad_profile"
test -f $CAD_PROFILE_FILE || cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE $TARGET_RESIDUE_IDS_FILE | $VOROPROT --mode calculate-contact-area-difference-profile --scoring-mode $SCORING_MODE > $CAD_PROFILE_FILE 2> "$CAD_PROFILE_FILE.log"

GLOBAL_SCORES_FILE="$OUTPUT_DIRECTORY/global_scores"
test -f $GLOBAL_SCORES_FILE || cat $CAD_PROFILE_FILE | $VOROPROT --mode calculate-contact-area-difference-global-score --use-min 1 > $GLOBAL_SCORES_FILE 2> "$GLOBAL_SCORES_FILE.log"
