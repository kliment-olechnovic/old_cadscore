#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -f    path to input file in PDB format
  -a    face coloring mode (residue_hydrophobicity, residue_type or atom_type) (optional)
  -b    selections coloring mode (residue_hydrophobicity, residue_type or atom_type) (optional)
  -c    path to CAD-score combined inter residue contacts file (optional)
  -g    residue groups (optional)

EOF
}

SCRIPT_DIRECTORY=$(dirname $0)
VOROPROT_NAME="voroprot2"
VOROPROT="$SCRIPT_DIRECTORY/$VOROPROT_NAME"
if [ ! -f "$VOROPROT" ]
then
  if which $VOROPROT_NAME &> /dev/null
  then
    VOROPROT=$VOROPROT_NAME
  else
    echo "Fatal error: '$VOROPROT_NAME' executable not found" 1>&2
    exit 1
  fi
fi

INPUT_FILE=""
FACE_COLORING_MODE=""
SELECTION_COLORING_MODE=""
COMBINED_RESIDUE_CONTACTS_FILE=""
RESIDUE_GROUPS=""

while getopts "hf:a:b:c:g:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    f)
      INPUT_FILE=$OPTARG
      ;;
    a)
      FACE_COLORING_MODE="--face-coloring "$OPTARG
      ;;
    b)
      SELECTION_COLORING_MODE="--selection-coloring "$OPTARG
      ;;
    c)
      COMBINED_RESIDUE_CONTACTS_FILE=$OPTARG
      ;;
    g)
      RESIDUE_GROUPS=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$INPUT_FILE" ]
then
  echo "Input file name not provided" 1>&2
  exit 1
fi

if [ ! -f "$INPUT_FILE" ]
then
  echo "Input file \"$INPUT_FILE\" does not exist" 1>&2
  exit 1
fi

TMP_DIR=$(mktemp -d)

USABLE_INPUT_FILE="$TMP_DIR/"$(basename $INPUT_FILE)
if [ -z "$RESIDUE_GROUPS" ]
then
  cp "$INPUT_FILE" "$USABLE_INPUT_FILE"
else
  $VOROPROT --mode reassign-chain-names-by-residue-intervals --intervals "$RESIDUE_GROUPS" < "$INPUT_FILE" > "$USABLE_INPUT_FILE"
  if [ ! -s "$USABLE_INPUT_FILE" ]
  then
    echo "Failed to reassign chain names by residue groups, check the '-g' parameter" 1>&2
    rm "$TMP_DIR"
    exit 1
  fi 
fi

SCRIPT_FILE="$TMP_DIR/script.py"

if [ -z "$COMBINED_RESIDUE_CONTACTS_FILE" ]
then
  $VOROPROT --mode collect-atoms < "$USABLE_INPUT_FILE" | $VOROPROT --mode print-inter-chain-interface-graphics $FACE_COLORING_MODE $SELECTION_COLORING_MODE > "$SCRIPT_FILE"
else
  ( $VOROPROT --mode collect-atoms < "$USABLE_INPUT_FILE" ; cat "$COMBINED_RESIDUE_CONTACTS_FILE" ) | $VOROPROT --mode print-inter-chain-interface-graphics --face-coloring inter_residue_contact_scores $SELECTION_COLORING_MODE > "$SCRIPT_FILE"
fi

if [ -s "$SCRIPT_FILE" ]
then
  pymol "$USABLE_INPUT_FILE" "$SCRIPT_FILE"
fi

rm -r "$TMP_DIR"
