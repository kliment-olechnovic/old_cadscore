#!/bin/bash

#This script accepts a single PDB filename from the command line

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

INPUT_FILE=$1

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

COLORING_MODE=""
if [ ! -z "$2" ]
then
  COLORING_MODE="--coloring $2"
fi

TMP_FILE=$(mktemp)
rm "$TMP_FILE"
SCRIPT_FILE="$TMP_FILE.py"

$VOROPROT --mode collect-atoms < "$INPUT_FILE" | $VOROPROT --mode print-inter-chain-interface-graphics $COLORING_MODE > "$SCRIPT_FILE"

pymol "$INPUT_FILE" "$SCRIPT_FILE"
rm "$SCRIPT_FILE"