#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -i    path to input file in PDB format
  -f    face coloring mode
  -s    selections coloring mode (optional)
  -g    residue groups description (optional)
  -p    flag to automatically open pymol and load the input PDB file and the produced sript (optional)
  -n    output names prefix (optional)
  
What this script does:
  It runs CAD-score voroprot2 program to produce a PyMol (http://pymol.org/) API script
  that draws Voronoi contact faces and, optionally, selects the contacting residues.
  
Running example:
  ./Voroprot2_print_inter_chain_interfaces.bash -i ~/Downloads/2ZSK.pdb -f residue_type -s residue_type -g "(A37-A44)(B37-B44)" -p
  
General notes:
  To run this script you need 'voroprot2' in your binary bath or in the same directory as this script.
  If '-g' option is not provided, inter-chain contacts are considered.
  If '-s' option is not provided, selections are not created.
  To use '-p' option you need 'pymol' in your binary path.

Describing residue groups:
  For example, the string "(A1-A99,B130-B170)(C15)(D)(3-81)" describes
  the contacts between the following four groups of residues:
    Group (A1-A99, B1-B99) stands for the residues from 1 to 99 in chain A and the residues from 130 to 170 in the chain B
    Group (C15, C45) stands for the residues 15 and 45 in the chain C
    Group (D) stands for all the residues in the chain D
    Group (3-81) stands for the residues from 3 to 81 in the unnamed chain 

Available coloring modes for faces and selections:
  blank
  residue_hydrophobicity
  residue_type
  atom_type
  residue_id
  atom_id

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
COMBINED_RESIDUE_CONTACTS_FILE_USAGE=""
RESIDUE_GROUPS=""
OUTPUT_NAMES_PREFIX=""
OPEN_IN_PYMOL=false

while getopts "hi:f:s:c:d:v:w:g:n:p" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    i)
      INPUT_FILE=$OPTARG
      ;;
    f)
      FACE_COLORING_MODE="--face-coloring "$OPTARG
      ;;
    s)
      SELECTION_COLORING_MODE="--selection-coloring "$OPTARG
      ;;
    c)
      COMBINED_RESIDUE_CONTACTS_FILE=$OPTARG
      COMBINED_RESIDUE_CONTACTS_FILE_USAGE="inter_residue_contact_scores"
      ;;
    d)
      COMBINED_RESIDUE_CONTACTS_FILE=$OPTARG
      COMBINED_RESIDUE_CONTACTS_FILE_USAGE="inter_residue_contact_AA_scores"
      ;;
    v)
      COMBINED_RESIDUE_CONTACTS_FILE=$OPTARG
      COMBINED_RESIDUE_CONTACTS_FILE_USAGE="inter_residue_contact_area_pairs"
      ;;
    w)
      COMBINED_RESIDUE_CONTACTS_FILE=$OPTARG
      COMBINED_RESIDUE_CONTACTS_FILE_USAGE="inter_residue_contact_AA_area_pairs"
      ;;
    g)
      RESIDUE_GROUPS="--groups "$OPTARG
      ;;
    n)
      OUTPUT_NAMES_PREFIX="--output-names-prefix "$OPTARG
      ;;
    p)
      OPEN_IN_PYMOL=true
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$INPUT_FILE" ]
then
  echo "Fatal error: input file name not provided" 1>&2
  exit 1
fi

if [ ! -f "$INPUT_FILE" ]
then
  echo "Fatal error: input file \"$INPUT_FILE\" does not exist" 1>&2
  exit 1
fi

TMP_DIR=$(mktemp -d)

SCRIPT_FILE="$TMP_DIR/script.py"

if [ -z "$COMBINED_RESIDUE_CONTACTS_FILE" ]
then
  $VOROPROT --mode collect-atoms < "$INPUT_FILE" | $VOROPROT --mode print-inter-chain-interface-graphics $FACE_COLORING_MODE $SELECTION_COLORING_MODE $RESIDUE_GROUPS $OUTPUT_NAMES_PREFIX > "$SCRIPT_FILE"
else
  ( $VOROPROT --mode collect-atoms < "$INPUT_FILE" ; cat "$COMBINED_RESIDUE_CONTACTS_FILE" ) | $VOROPROT --mode print-inter-chain-interface-graphics --face-coloring $COMBINED_RESIDUE_CONTACTS_FILE_USAGE $SELECTION_COLORING_MODE $RESIDUE_GROUPS $OUTPUT_NAMES_PREFIX > "$SCRIPT_FILE"
fi

if [ -s "$SCRIPT_FILE" ]
then
  if $OPEN_IN_PYMOL
  then
    if which pymol &> /dev/null
    then
      pymol "$INPUT_FILE" "$SCRIPT_FILE"
    else
      echo "'pymol' executable not found" 1>&2
    fi
  else
    cat $SCRIPT_FILE
  fi
else
  echo "PyMol script file was not produced" 1>&2
fi

rm -r "$TMP_DIR"