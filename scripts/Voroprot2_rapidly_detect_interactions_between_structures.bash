#!/bin/bash

##################################################
### Printing usage help

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -p    probe radius (optional, default is 1.4 angstroms)
  -a    flag to read heteroatoms (optional)

What this script does:
  It runs CAD-score voroprot2 program to rapidly detect if there are any interactions between two or more PDB structures.
  
General notes:
  The list of PDB files paths is accepted from the standard input.
  To run this script you need 'voroprot2' in your binary bath or in the same directory as this script.

EOF
}

##################################################
### Reading and checking arguments

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

PROBE_RADIUS=""
HETATM_FLAG=""

while getopts "hp:a" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    p)
      PROBE_RADIUS="--probe "$OPTARG
      ;;
    a)
      HETATM_FLAG="--read-heteroatoms"
      ;;
    ?)
      exit 1
      ;;
  esac
done

##################################################
### Running detection

$VOROPROT --mode check-for-any-inter-chain-contact --direct-reading $HETATM_FLAG $PROBE_RADIUS
