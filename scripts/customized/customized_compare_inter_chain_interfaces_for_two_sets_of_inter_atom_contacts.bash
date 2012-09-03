#!/bin/bash

VOROPROT=$1
TARGET_CONTACTS_FILE=$2
MODEL_CONTACTS_FILE=$3

if which $VOROPROT &> /dev/null
then
  true
else
  echo "Fatal error: '$VOROPROT' executable not found" 1>&2
  exit 1
fi

if [ ! -f "$TARGET_CONTACTS_FILE" ]
then
  echo "Fatal error: 'A' input file '$TARGET_CONTACTS_FILE' does not exist" 1>&2
  exit 1
fi

if [ ! -f "$MODEL_CONTACTS_FILE" ]
then
  echo "Fatal error: 'B' input file '$MODEL_CONTACTS_FILE' does not exist" 1>&2
  exit 1
fi

( \
 cat $TARGET_CONTACTS_FILE $MODEL_CONTACTS_FILE | $VOROPROT --mode calc-combined-inter-residue-contacts-with-chains-optimally-renamed --inter-chain ; \
 cat $TARGET_CONTACTS_FILE | $VOROPROT --mode collect-residue-ids \
) \
| $VOROPROT --mode calc-CAD-profile \
| $VOROPROT --mode calc-CAD-global-scores \
| egrep -v 'W'
