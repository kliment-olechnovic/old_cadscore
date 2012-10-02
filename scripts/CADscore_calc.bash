#!/bin/bash

##################################################
### Help printing

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -t    path to target file in PDB format
  -m    path to model file in PDB format
  -l    flag to include heteroatoms (optional)
  -v    path to atomic radii files directory (optional)
  -c    flag to consider only inter-chain contacts (optional)
  -i    inter-interval contacts specification (optional)
  -o    max timeout (optional)
  -g    flag to use TM-score (optional)
  -u    flag to disable model atoms filtering by target atoms (optional)
  -q    flag to try to rename chains for best possible scores (optional)

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

DATABASE=""
TARGET_FILE=""
MODEL_FILE=""
HETATM_FLAG=""
RADII_OPTION=""
INTER_CHAIN_FLAG=""
INTER_INTERVAL_OPTION=""
TIMEOUT="300s"
USE_TMSCORE=false
DISABLE_MODEL_ATOMS_FILTERING=false
QUATERNARY_CHAINS_RENAMING=false

while getopts "hD:t:m:lv:ci:o:guq" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    t)
      TARGET_FILE=$OPTARG
      ;;
    m)
      MODEL_FILE=$OPTARG
      ;;
    l)
      HETATM_FLAG="--HETATM"
      ;;
    v)
      RADII_OPTION="--radius-classes $OPTARG/vdwr_classes --radius-members $OPTARG/vdwr_members"
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
    g)
      USE_TMSCORE=true
      ;;
    u)
      DISABLE_MODEL_ATOMS_FILTERING=true
      ;;
    q)
      QUATERNARY_CHAINS_RENAMING=true
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ] || [ -z "$TARGET_FILE" ] || [ -z "$MODEL_FILE" ]
then
  print_help
  exit 1
fi

if [ -f "$TARGET_FILE" ]
then
  TARGET_NAME=$(basename $TARGET_FILE)
else
  echo "Fatal error: target file \"$TARGET_FILE\" does not exist" 1>&2
  exit 1
fi

if [ -f "$MODEL_FILE" ]
then
  MODEL_NAME=$(basename $MODEL_FILE)
else
  echo "Fatal error: model file \"$MODEL_FILE\" does not exist" 1>&2
  exit 1
fi

##################################################
### Preparing and checking environment

TARGET_DIR="$DATABASE/$TARGET_NAME"
TARGET_PARAMETERS_FILE="$TARGET_DIR/parameters"
TARGET_INTER_ATOM_CONTACTS_FILE="$TARGET_DIR/inter_atom_contacts"
TARGET_RESIDUE_IDS_FILE="$TARGET_DIR/residue_ids"
TARGET_INTER_RESIDUE_CONTACTS_FILE="$TARGET_DIR/inter_residue_contacts"
MODEL_DIR="$DATABASE/$TARGET_NAME/$MODEL_NAME"
MODEL_INTER_ATOM_CONTACTS_FILE="$MODEL_DIR/inter_atom_contacts"
MODEL_RESIDUE_IDS_FILE="$MODEL_DIR/residue_ids"
COMBINED_INTER_RESIDUE_CONTACTS_FILE="$MODEL_DIR/combined_inter_residue_contacts"
CAD_PROFILE_FILE="$MODEL_DIR/cad_profile"
CAD_GLOBAL_SCORES_FILE="$MODEL_DIR/cad_global_scores"
CAD_SIZE_SCORES_FILE="$MODEL_DIR/cad_size_scores"
TMSCORE_PROFILE_FILE="$MODEL_DIR/tmscore_profile"
TMSCORE_GLOBAL_SCORES_FILE="$MODEL_DIR/tmscore_global_scores"
SUMMARY_FILE="$MODEL_DIR/summary"
TARGET_LOGS_FILE="$TARGET_DIR/target_logs"
MODEL_LOGS_FILE="$MODEL_DIR/model_logs"

mkdir -p "$TARGET_DIR"
mkdir -p "$MODEL_DIR"

TARGET_PARAMETERS="$HETATM_FLAG $RADII_OPTION $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION"
if [ -f "$TARGET_PARAMETERS_FILE" ]
then
  CURRENT_TARGET_PARAMETERS=$(cat $TARGET_PARAMETERS_FILE)
  if [ "$TARGET_PARAMETERS" != "$CURRENT_TARGET_PARAMETERS" ]
  then
    echo "Fatal error: current parameters ($TARGET_PARAMETERS) do not match the initial parameters" 1>&2
    exit 1
  fi
else
  echo "$TARGET_PARAMETERS" > $TARGET_PARAMETERS_FILE
fi

##################################################
### Preprocessing target

test -f $TARGET_INTER_ATOM_CONTACTS_FILE || cat $TARGET_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG $RADII_OPTION | timeout $TIMEOUT $VOROPROT --mode calc-inter-atom-contacts > $TARGET_INTER_ATOM_CONTACTS_FILE
if [ ! -s "$TARGET_INTER_ATOM_CONTACTS_FILE" ] ; then echo "Fatal error: no inter-atom contacts in the target" 1>&2 ; exit 1 ; fi

test -f $TARGET_RESIDUE_IDS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode collect-residue-ids  > $TARGET_RESIDUE_IDS_FILE
if [ ! -s "$TARGET_RESIDUE_IDS_FILE" ] ; then echo "Fatal error: no residues in the target" 1>&2 ; exit 1 ; fi

test -f $TARGET_INTER_RESIDUE_CONTACTS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-inter-residue-contacts $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION > $TARGET_INTER_RESIDUE_CONTACTS_FILE
if [ ! -s "$TARGET_INTER_RESIDUE_CONTACTS_FILE" ] ; then echo "Fatal error: no inter-residue contacts in the target" 1>&2 ; exit 1 ; fi

##################################################
### Preprocessing model

if $DISABLE_MODEL_ATOMS_FILTERING
then
  test -f $MODEL_INTER_ATOM_CONTACTS_FILE || cat $MODEL_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG $RADII_OPTION | timeout $TIMEOUT $VOROPROT --mode calc-inter-atom-contacts > $MODEL_INTER_ATOM_CONTACTS_FILE
else
  test -f $MODEL_INTER_ATOM_CONTACTS_FILE || (cat $MODEL_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG $RADII_OPTION ; cat $TARGET_INTER_ATOM_CONTACTS_FILE) | $VOROPROT --mode filter-atoms-by-target | timeout $TIMEOUT $VOROPROT --mode calc-inter-atom-contacts > $MODEL_INTER_ATOM_CONTACTS_FILE
fi

if [ ! -s "$MODEL_INTER_ATOM_CONTACTS_FILE" ] ; then echo "Fatal error: no inter-atom contacts in the model" 1>&2 ; exit 1 ; fi

test -f $MODEL_RESIDUE_IDS_FILE || cat $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode collect-residue-ids  > $MODEL_RESIDUE_IDS_FILE
if [ ! -s "$MODEL_RESIDUE_IDS_FILE" ] ; then echo "Fatal error: no filtered residues in the model" 1>&2 ; exit 1 ; fi

##################################################
### Comparing target and model

if $QUATERNARY_CHAINS_RENAMING
then
  test -f $COMBINED_INTER_RESIDUE_CONTACTS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-combined-inter-residue-contacts-with-chains-optimally-renamed $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION > $COMBINED_INTER_RESIDUE_CONTACTS_FILE
else
  test -f $COMBINED_INTER_RESIDUE_CONTACTS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-combined-inter-residue-contacts $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION > $COMBINED_INTER_RESIDUE_CONTACTS_FILE
fi

if [ ! -s "$COMBINED_INTER_RESIDUE_CONTACTS_FILE" ] ; then echo "Fatal error: combined inter-residue contacts file is empty" 1>&2 ; exit 1 ; fi
	
test -f $CAD_PROFILE_FILE || cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE $TARGET_RESIDUE_IDS_FILE | $VOROPROT --mode calc-CAD-profile > $CAD_PROFILE_FILE
if [ ! -s "$CAD_PROFILE_FILE" ] ; then echo "Fatal error: CAD profile file is empty" 1>&2 ; exit 1 ; fi

test -f $CAD_GLOBAL_SCORES_FILE || cat $CAD_PROFILE_FILE | $VOROPROT --mode calc-CAD-global-scores > $CAD_GLOBAL_SCORES_FILE
if [ ! -s "$CAD_GLOBAL_SCORES_FILE" ] ; then echo "Fatal error: CAD global scores file is empty" 1>&2 ; exit 1 ; fi
	
test -f $CAD_SIZE_SCORES_FILE || cat $CAD_PROFILE_FILE $TARGET_RESIDUE_IDS_FILE $MODEL_RESIDUE_IDS_FILE | $VOROPROT --mode calc-CAD-size-scores > $CAD_SIZE_SCORES_FILE
if [ ! -s "$CAD_SIZE_SCORES_FILE" ] ; then echo "Fatal error: CAD size scores file is empty" 1>&2 ; exit 1 ; fi

if $USE_TMSCORE
then
  TMSCORE_CALC_NAME="TMscore_calc.bash"
  TMSCORE_CALC="$SCRIPT_DIRECTORY/$TMSCORE_CALC_NAME"
  if [ ! -f "$TMSCORE_CALC" ]
  then
    if which $TMSCORE_CALC_NAME &> /dev/null
    then
      TMSCORE_CALC=$TMSCORE_CALC_NAME
    else
      echo "Fatal error: '$TMSCORE_CALC_NAME' script not found" 1>&2
      exit 1
    fi
  fi
  test -f $TMSCORE_GLOBAL_SCORES_FILE || $TMSCORE_CALC -m $MODEL_FILE -t $TARGET_FILE -p $TMSCORE_PROFILE_FILE -s $TMSCORE_GLOBAL_SCORES_FILE
  if [ ! -s "$TMSCORE_GLOBAL_SCORES_FILE" ] ; then echo "Fatal error: TM-score scores file is empty" 1>&2 ; exit 1 ; fi
fi

##################################################
### Writing global results

echo target $TARGET_NAME > $SUMMARY_FILE
echo model $MODEL_NAME >> $SUMMARY_FILE
cat $CAD_SIZE_SCORES_FILE >> $SUMMARY_FILE
cat $CAD_GLOBAL_SCORES_FILE | grep -v "_diff" | grep -v "_ref" | grep -v "W" >> $SUMMARY_FILE
if $USE_TMSCORE ; then cat $TMSCORE_GLOBAL_SCORES_FILE >> $SUMMARY_FILE ; fi
