#!/bin/bash

VOROPROT=$1
PDB_FILE=$2

if which $VOROPROT &> /dev/null
then
  true
else
  echo "Fatal error: '$VOROPROT' executable not found" 1>&2
  exit 1
fi

if [ ! -f "$PDB_FILE" ]
then
  echo "Fatal error: input file '$PDB_FILE' does not exist" 1>&2
  exit 1
fi

./voroprot2 --mode collect-atoms --HETATM < $PDB_FILE \
| ./voroprot2 --mode check-for-inter-atom-clashes --inter-chain --cutoff 1.0
