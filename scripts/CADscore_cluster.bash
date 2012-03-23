#!/bin/bash

##################################################
### Help printing

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -I    path to input directory
  -D    path to writable output directory
  -l    flag to include heteroatoms (optional)
  -c    flag to consider only inter-chain contacts (optional)
  -i    inter-interval contacts specification (optional)
  -o    max timeout (optional)

EOF
}

##################################################
### Reading and checking arguments

SCRIPT_DIRECTORY=$(dirname $0)

VOROPROT="$SCRIPT_DIRECTORY/voroprot2"
if [ ! -f "$VOROPROT" ]
then
  echo "$VOROPROT does not exist" 1>&2
  exit 1
fi

INPUT_DIR=""
DATABASE=""
HETATM_FLAG=""
INTER_CHAIN_FLAG=""
INTER_INTERVAL_OPTION=""
TIMEOUT="300s"
USE_TMSCORE=false

while getopts "hI:D:lci:o:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    I)
      INPUT_DIR=$OPTARG
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    l)
      HETATM_FLAG="--HETATM"
      ;;
    c)
      INTER_CHAIN_FLAG="--inter-chain"
      ;;
    i)
      INTER_INTERVAL_OPTION="--inter-interval "$OPTARG
      ;;
    o)
      TIMEOUT=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$INPUT_DIR" ] || [ -z "$DATABASE" ]
then
  print_help
  exit 1
fi

if [ ! -d "$INPUT_DIR" ]
then
  echo "Fatal error: input directory \"$INPUT_DIR\" does not exist" 1>&2
  exit 1
fi

find "$INPUT_DIR" -mindepth 1 -maxdepth 1 -type f | sort | while read TARGET_FILE
do
  TARGET_NAME=$(basename $TARGET_FILE)
  TARGET_DIR="$DATABASE/$TARGET_NAME"
  TARGET_INTER_ATOM_CONTACTS_FILE="$TARGET_DIR/inter_atom_contacts"
  TARGET_RESIDUE_IDS_FILE="$TARGET_DIR/residue_ids"
  
  mkdir -p "$TARGET_DIR"
  
  test -f $TARGET_INTER_ATOM_CONTACTS_FILE || cat $TARGET_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG | timeout $TIMEOUT $VOROPROT --mode calc-inter-atom-contacts > $TARGET_INTER_ATOM_CONTACTS_FILE 2> $TARGET_INTER_ATOM_CONTACTS_FILE.log
  if [ ! -s "$TARGET_INTER_ATOM_CONTACTS_FILE" ] || [ ! "$(sed -n '2p' $TARGET_INTER_ATOM_CONTACTS_FILE)" -gt "0" ] ; then echo "Fatal error: no inter-atom contacts in the target" 1>&2 ; exit 1 ; fi

  test -f $TARGET_RESIDUE_IDS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode collect-residue-ids  > $TARGET_RESIDUE_IDS_FILE 2> $TARGET_RESIDUE_IDS_FILE.log
  if [ ! -s "$TARGET_RESIDUE_IDS_FILE" ] || [ ! "$(sed -n '2p' $TARGET_RESIDUE_IDS_FILE)" -gt "0" ] ; then echo "Fatal error: no residues in the target" 1>&2 ; exit 1 ; fi
done

find "$INPUT_DIR" -mindepth 1 -maxdepth 1 -type f | sort | while read TARGET_FILE
do
  TARGET_NAME=$(basename $TARGET_FILE)
  TARGET_DIR="$DATABASE/$TARGET_NAME"
  TARGET_INTER_ATOM_CONTACTS_FILE="$TARGET_DIR/inter_atom_contacts"
  TARGET_RESIDUE_IDS_FILE="$TARGET_DIR/residue_ids"
  
  find "$INPUT_DIR" -mindepth 1 -maxdepth 1 -type f | sort | while read MODEL_FILE
  do
  	MODEL_NAME=$(basename $MODEL_FILE)
  	MODEL_INTER_ATOM_CONTACTS_FILE="$DATABASE/$MODEL_NAME/inter_atom_contacts"
  	
  	echo target $TARGET_NAME
    echo model $MODEL_NAME
  	( cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-combined-inter-residue-contacts $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION ; cat $TARGET_RESIDUE_IDS_FILE ) | $VOROPROT --mode calc-CAD-profile | $VOROPROT --mode calc-CAD-global-scores | grep -v "_diff" | grep -v "_ref"
  done
done
