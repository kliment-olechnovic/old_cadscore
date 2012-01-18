#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

VOROPROT=$SCRIPT_DIRECTORY/voroprot2

INCLUDE_HETEROATOMS="0"
FORCE_ONE_CHAIN="1"

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -i    path to input file in PBD format
  -t    target name in the database to be used for filtering (optional)
  
EOF
}

DATABASE=""
INPUT_FILE=""
TARGET_NAME=""

while getopts "hD:i:t:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    i)
      INPUT_FILE=$OPTARG
      ;;
    t)
      TARGET_NAME=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ] || [ -z "$INPUT_FILE" ]
then
  print_help
  exit 1
fi

if [ -f "$INPUT_FILE" ]
then
  INPUT_NAME=$(basename $INPUT_FILE)
else
  echo "Input file \"$INPUT_FILE\" does not exist" 1>&2
  exit 1
fi

if [ -n "$TARGET_NAME" ]
then
  TARGET_ATOMS_FILE="$DATABASE/atoms/$TARGET_NAME/atoms"
  if [ ! -f "$TARGET_ATOMS_FILE" ]
  then
    echo "Atoms file for target \"$TARGET_NAME\" is not in the database" 1>&2
    exit 1
  fi
fi

###########################################

OUTPUT_DIRECTORY="$DATABASE/atoms/$INPUT_NAME"
mkdir -p $OUTPUT_DIRECTORY

ATOMS_FILE="$OUTPUT_DIRECTORY/atoms"
test -f $ATOMS_FILE || cat $INPUT_FILE | $VOROPROT --mode collect-atoms --include-heteroatoms $INCLUDE_HETEROATOMS --include-water 0 --force-one-chain $FORCE_ONE_CHAIN > $ATOMS_FILE 2> "$ATOMS_FILE.log"

if [ -n "$TARGET_NAME" ]
then
  OUTPUT_DIRECTORY="$OUTPUT_DIRECTORY/$TARGET_NAME"
  mkdir -p $OUTPUT_DIRECTORY
  
  SELECTED_ATOMS_FILE="$OUTPUT_DIRECTORY/atoms"
  test -f $SELECTED_ATOMS_FILE || cat $TARGET_ATOMS_FILE $ATOMS_FILE | $VOROPROT --mode filter-atoms-by-target > $SELECTED_ATOMS_FILE 2> "$SELECTED_ATOMS_FILE.log"
  
  ATOMS_FILE=$SELECTED_ATOMS_FILE
fi

RESIDUE_IDS_FILE="$OUTPUT_DIRECTORY/residue_ids"
test -f $RESIDUE_IDS_FILE || cat $ATOMS_FILE | $VOROPROT --mode collect-residue-ids  > $RESIDUE_IDS_FILE 2> "$RESIDUE_IDS_FILE.log"
