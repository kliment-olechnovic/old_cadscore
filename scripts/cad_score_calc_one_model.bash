#!/bin/bash

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

###########################################

SCRIPT_DIRECTORY=$(dirname $0)
VOROPROT="$SCRIPT_DIRECTORY/voroprot2"

if [ ! -f "$TARGET_FILE" ]
then
  echo "Voroprot executable not found in the script deirectory" 1>&2
  exit 1
fi

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

TARGET_DIR="$DATABASE/$TARGET_NAME"
TARGET_ATOMS_FILE="$TARGET_DIR/atoms"
TARGET_RESIDUE_IDS_FILE="$TARGET_DIR/residue_ids"
TARGET_INTER_ATOM_CONTACTS_FILE="$TARGET_DIR/inter_atom_contacts"

MODEL_DIR="$DATABASE/$TARGET_NAME/$MODEL_NAME"
MODEL_ATOMS_FILE="$MODEL_DIR/atoms"
MODEL_RESIDUE_IDS_FILE="$MODEL_DIR/residue_ids"
MODEL_INTER_ATOM_CONTACTS_FILE="$MODEL_DIR/inter_atom_contacts"

COMBINED_INTER_RESIDUE_CONTACTS_FILE="$MODEL_DIR/combined_inter_residue_contacts"
CAD_PROFILE_FILE="$MODEL_DIR/cad_profile"
CAD_GLOBAL_SCORES_FILE="$MODEL_DIR/cad_global_scores"

mkdir -p "$TARGET_DIR"
mkdir -p "$MODEL_DIR"

test -f $TARGET_ATOMS_FILE || cat $TARGET_FILE | $VOROPROT --mode collect-atoms --force-one-chain > "$TARGET_ATOMS_FILE" 2> "$TARGET_ATOMS_FILE.log"
test -f $TARGET_RESIDUE_IDS_FILE || cat $TARGET_ATOMS_FILE | $VOROPROT --mode collect-residue-ids  > "$TARGET_RESIDUE_IDS_FILE" 2> "$TARGET_RESIDUE_IDS_FILE.log"
test -f $TARGET_INTER_ATOM_CONTACTS_FILE || cat $TARGET_ATOMS_FILE | timeout 300s $VOROPROT --mode calc-inter-atom-contacts > "$TARGET_INTER_ATOM_CONTACTS_FILE" 2> "$TARGET_INTER_ATOM_CONTACTS_FILE.log"

if [ ! -f $MODEL_ATOMS_FILE ]
then
  cat $MODEL_FILE | $VOROPROT --mode collect-atoms --force-one-chain > "$MODEL_ATOMS_FILE" 2> "$MODEL_ATOMS_FILE.log"
  cat $TARGET_ATOMS_FILE $MODEL_ATOMS_FILE | $VOROPROT --mode filter-atoms-by-target > "$MODEL_ATOMS_FILE.filtered" 2>> "$MODEL_ATOMS_FILE.log"
  mv "$MODEL_ATOMS_FILE.filtered" $MODEL_ATOMS_FILE
fi
test -f $MODEL_RESIDUE_IDS_FILE || cat $MODEL_ATOMS_FILE | $VOROPROT --mode collect-residue-ids  > "$MODEL_RESIDUE_IDS_FILE" 2> "$MODEL_RESIDUE_IDS_FILE.log"
test -f $MODEL_INTER_ATOM_CONTACTS_FILE || cat $MODEL_ATOMS_FILE | timeout 300s $VOROPROT --mode calc-inter-atom-contacts > "$MODEL_INTER_ATOM_CONTACTS_FILE" 2> "$MODEL_INTER_ATOM_CONTACTS_FILE.log"

test -f $COMBINED_INTER_RESIDUE_CONTACTS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-combined-inter-residue-contacts > $COMBINED_INTER_RESIDUE_CONTACTS_FILE 2> "$COMBINED_INTER_RESIDUE_CONTACTS_FILE.log"
test -f $CAD_PROFILE_FILE || cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE $TARGET_RESIDUE_IDS_FILE | $VOROPROT --mode calc-CAD-profile > $CAD_PROFILE_FILE 2> "$CAD_PROFILE_FILE.log"
test -f $CAD_GLOBAL_SCORES_FILE || cat $CAD_PROFILE_FILE | $VOROPROT --mode calc-CAD-global-scores > $CAD_GLOBAL_SCORES_FILE 2> "$CAD_GLOBAL_SCORES_FILE.log"
