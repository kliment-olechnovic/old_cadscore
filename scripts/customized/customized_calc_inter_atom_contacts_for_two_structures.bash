#!/bin/bash

VOROPROT=$1
PDB_FILE_A=$2
PDB_FILE_B=$3

if which $VOROPROT &> /dev/null
then
  true
else
  echo "Fatal error: '$VOROPROT' executable not found" 1>&2
  exit 1
fi

if [ ! -f "$PDB_FILE_A" ]
then
  echo "Fatal error: 'A' input file '$PDB_FILE_A' does not exist" 1>&2
  exit 1
fi

if [ ! -f "$PDB_FILE_B" ]
then
  echo "Fatal error: 'B' input file '$PDB_FILE_B' does not exist" 1>&2
  exit 1
fi

($VOROPROT --mode collect-atoms --HETATM --rename-chain A < $PDB_FILE_A ; \
$VOROPROT --mode collect-atoms --HETATM --rename-chain B < $PDB_FILE_B) \
| $VOROPROT --mode merge-atoms \
| $VOROPROT --mode calc-inter-atom-contacts
