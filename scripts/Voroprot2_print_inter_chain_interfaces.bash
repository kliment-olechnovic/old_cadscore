#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -f    path to input file in PDB format
  -c    coloring mode (residue_hydrophobicity or residue_type) (optional)
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
COLORING_MODE=""
RESIDUE_GROUPS=""

while getopts "hf:c:g:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    f)
      INPUT_FILE=$OPTARG
      ;;
    c)
      COLORING_MODE="--coloring "$OPTARG
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

if $VOROPROT --mode collect-atoms < "$USABLE_INPUT_FILE" | $VOROPROT --mode print-inter-chain-interface-graphics $COLORING_MODE > "$SCRIPT_FILE"
then
  pymol "$USABLE_INPUT_FILE" "$SCRIPT_FILE"
fi

rm "$TMP_DIR"
