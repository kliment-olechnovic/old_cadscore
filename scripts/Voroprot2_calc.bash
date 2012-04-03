#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -f    path to input file in PBD format
  -l    flag to include heteroatoms (optional)
  -a    flag to print atoms (optional)
  -q    flag to print quadruples (optional)
  -c    flag to print inter-atom contacts (optional)
  -r    flag to print inter-residue contacts (optional)

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

MODEL_FILE=""
HETATM_FLAG=""
PRINT_ATOMS=false
QUADRUPLES=false
INTER_ATOM_CONTACTS=false
INTER_RESIDUE_CONTACTS=false

while getopts "hf:laqcr" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    f)
      MODEL_FILE=$OPTARG
      ;;
    l)
      HETATM_FLAG="--HETATM"
      ;;
    a)
      PRINT_ATOMS=true
      ;;
    q)
      QUADRUPLES=true
      ;;
    c)
      INTER_ATOM_CONTACTS=true
      ;;
    r)
      INTER_RESIDUE_CONTACTS=true
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$MODEL_FILE" ]
then
  print_help
  exit 1
fi

if [ ! -f "$MODEL_FILE" ]
then
  echo "File \"$MODEL_FILE\" does not exist" 1>&2
  exit 1
fi

if $PRINT_ATOMS
then
  cat $MODEL_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG
fi

if $QUADRUPLES
then
  cat $MODEL_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG | $VOROPROT --mode calc-quadruples
fi

if $INTER_ATOM_CONTACTS
then
  cat $MODEL_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG | $VOROPROT --mode calc-inter-atom-contacts
fi

if $INTER_RESIDUE_CONTACTS
then
  cat $MODEL_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG | $VOROPROT --mode calc-inter-atom-contacts | $VOROPROT --mode calc-inter-residue-contacts
fi
