#!/bin/bash

##################################################
### Help printing

print_help()
{
cat << EOF 1>&2

$0 parameters:

  Required:
    -D    path to writable database directory
    -t    path to target file in PDB format
    -m    path to model file in PDB format

  Optional (basic):
    -l    flag to include heteroatoms
    -c    flag to consider only inter-chain contacts
    -i    inter-interval contacts specification
    -q    flag to try to rename chains for best possible scores
    -g    flag to use TM-score

  Optional (advanced):  
    -a    flag to compute atomic global scores
    -u    flag to disable model atoms filtering by target atoms
    -o    max heavy operation timeout (seconds)
    -v    path to atomic radii files directory
    -e    extra command to produce additional global scores
    -j    turn off thread-safe mode
    
  Other:
    -h    show this message and exit


A brief tutorial is available from CAD-score site:

  https://bitbucket.org/kliment/cadscore/wiki/Home

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
TIMEOUT="300"
USE_TMSCORE=false
DISABLE_MODEL_ATOMS_FILTERING=false
QUATERNARY_CHAINS_RENAMING=false
EXTRA_COMMAND=""
USE_ATOMIC_CADSCORE=false
THREAD_SAFE_ON=true

while getopts "hD:t:m:lv:ci:o:guqe:aj" OPTION
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
    e)
      EXTRA_COMMAND=$OPTARG
      ;;
    a)
      USE_ATOMIC_CADSCORE=true
      ;;
    j)
      THREAD_SAFE_ON=false
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ] || [ -z "$TARGET_FILE" ] || [ -z "$MODEL_FILE" ]
then
  echo "Fatal error: required parameters were not provided" 1>&2
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
TARGET_MUTEX_END="$TARGET_DIR/mutex_closed"
TARGET_PARAMETERS_FILE="$TARGET_DIR/parameters"
TARGET_ATOMS_FILE="$TARGET_DIR/atoms"
TARGET_INTER_ATOM_CONTACTS_FILE="$TARGET_DIR/inter_atom_contacts"
TARGET_RESIDUE_IDS_FILE="$TARGET_DIR/residue_ids"
TARGET_INTER_RESIDUE_CONTACTS_FILE="$TARGET_DIR/inter_residue_contacts"
MODEL_DIR="$DATABASE/$TARGET_NAME/$MODEL_NAME"
MODEL_ATOMS_FILE="$MODEL_DIR/atoms"
MODEL_FILTERED_ATOMS_FILE="$MODEL_DIR/filtered_atoms"
MODEL_INTER_ATOM_CONTACTS_FILE="$MODEL_DIR/inter_atom_contacts"
MODEL_RESIDUE_IDS_FILE="$MODEL_DIR/residue_ids"
COMBINED_INTER_RESIDUE_CONTACTS_FILE="$MODEL_DIR/combined_inter_residue_contacts"
CAD_PROFILE_FILE="$MODEL_DIR/cad_profile"
CAD_GLOBAL_SCORES_FILE="$MODEL_DIR/cad_global_scores"
CAD_SIZE_SCORES_FILE="$MODEL_DIR/cad_size_scores"
CAD_ATOMIC_GLOBAL_SCORES_FILE="$MODEL_DIR/cad_atomic_global_scores"
TMSCORE_PROFILE_FILE="$MODEL_DIR/tmscore_profile"
TMSCORE_GLOBAL_SCORES_FILE="$MODEL_DIR/tmscore_global_scores"
EXTRA_COMMAND_GLOBAL_SCORES_FILE="$MODEL_DIR/extra_command_global_scores"
SUMMARY_FILE="$MODEL_DIR/summary"

TARGET_PARAMETERS="$HETATM_FLAG $RADII_OPTION $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION"

mkdir -p $DATABASE

##################################################
### Preprocessing target

if mkdir $TARGET_DIR &> /dev/null
then
  echo -n "$TARGET_PARAMETERS" > $TARGET_PARAMETERS_FILE
  
  if [ ! -f $TARGET_ATOMS_FILE ] ; then cat $TARGET_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG $RADII_OPTION > $TARGET_ATOMS_FILE ; fi
  if [ -s "$TARGET_ATOMS_FILE" ] && [ ! -f $TARGET_INTER_ATOM_CONTACTS_FILE ] ; then cat $TARGET_ATOMS_FILE | timeout $TIMEOUT"s" $VOROPROT --mode calc-inter-atom-contacts > $TARGET_INTER_ATOM_CONTACTS_FILE ; fi
  if [ -s "$TARGET_INTER_ATOM_CONTACTS_FILE" ] && [ ! -f $TARGET_RESIDUE_IDS_FILE ] ; then cat $TARGET_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode collect-residue-ids  > $TARGET_RESIDUE_IDS_FILE ; fi
  if [ -s "$TARGET_INTER_ATOM_CONTACTS_FILE" ] && [ ! -f $TARGET_INTER_RESIDUE_CONTACTS_FILE ] ; then  cat $TARGET_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-inter-residue-contacts $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION > $TARGET_INTER_RESIDUE_CONTACTS_FILE ; fi

  true > $TARGET_MUTEX_END
else
  if [ ! -d "$TARGET_DIR" ] ; then echo "Fatal error: could not create target directory ($TARGET_DIR)" 1>&2 ; exit 1 ; fi 
  
  if $THREAD_SAFE_ON
  then
    if [ ! -f "$TARGET_MUTEX_END" ]
    then
      WAITING_START_TIME=$(date +%s)
      while [ ! -f "$TARGET_MUTEX_END" ]
      do
  	    CURRENT_TIME=$(date +%s)
  	    WAITING_TIME=$((CURRENT_TIME-WAITING_START_TIME))
  	    if [ "$WAITING_TIME" -gt "$TIMEOUT" ]
  	    then
  	      echo "Fatal error: timeout expired when waiting for target processing" 1>&2
  	      exit 1
  	    fi
      done
    fi
  fi
  
  CURRENT_TARGET_PARAMETERS=$(< $TARGET_PARAMETERS_FILE)
  if [ "$TARGET_PARAMETERS" != "$CURRENT_TARGET_PARAMETERS" ]
  then
    echo "Fatal error: provided script parameters do not match the previous parameters that were used with the same target name and the same database path" 1>&2
    exit 1
  fi
fi

if [ ! -s "$TARGET_ATOMS_FILE" ] ; then echo "Fatal error: no atoms in the target" 1>&2 ; exit 1 ; fi
if [ ! -s "$TARGET_INTER_ATOM_CONTACTS_FILE" ] ; then echo "Fatal error: no inter-atom contacts in the target" 1>&2 ; exit 1 ; fi
if [ ! -s "$TARGET_RESIDUE_IDS_FILE" ] ; then echo "Fatal error: no residues in the target" 1>&2 ; exit 1 ; fi
if [ ! -s "$TARGET_INTER_RESIDUE_CONTACTS_FILE" ] ; then echo "Fatal error: no inter-residue contacts in the target" 1>&2 ; exit 1 ; fi

##################################################
### Preprocessing model

mkdir -p $MODEL_DIR

test -f $MODEL_ATOMS_FILE || cat $MODEL_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG $RADII_OPTION > $MODEL_ATOMS_FILE
if [ ! -s "$MODEL_ATOMS_FILE" ] ; then echo "Fatal error: no atoms in the model" 1>&2 ; exit 1 ; fi

if $DISABLE_MODEL_ATOMS_FILTERING
then
  MODEL_FILTERED_ATOMS_FILE=$MODEL_ATOMS_FILE
fi

test -f $MODEL_FILTERED_ATOMS_FILE || (cat $MODEL_ATOMS_FILE ; cat $TARGET_ATOMS_FILE) | $VOROPROT --mode filter-atoms-by-target > $MODEL_FILTERED_ATOMS_FILE
if [ ! -s "$MODEL_FILTERED_ATOMS_FILE" ] ; then echo "Fatal error: no atoms left in the model after filtering by target" 1>&2 ; exit 1 ; fi

test -f $MODEL_INTER_ATOM_CONTACTS_FILE || cat $MODEL_FILTERED_ATOMS_FILE | timeout $TIMEOUT"s" $VOROPROT --mode calc-inter-atom-contacts > $MODEL_INTER_ATOM_CONTACTS_FILE
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

if $USE_ATOMIC_CADSCORE
then
  test -f $CAD_ATOMIC_GLOBAL_SCORES_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-inter-atom-CAD-score --global $INTER_CHAIN_FLAG > $CAD_ATOMIC_GLOBAL_SCORES_FILE
  if [ ! -s "$CAD_ATOMIC_GLOBAL_SCORES_FILE" ] ; then echo "Fatal error: CAD atomic global scores file is empty" 1>&2 ; exit 1 ; fi
fi

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

if [ -n "$EXTRA_COMMAND" ]
then
  test -f $EXTRA_COMMAND_GLOBAL_SCORES_FILE || $EXTRA_COMMAND $TARGET_FILE $MODEL_FILE > $EXTRA_COMMAND_GLOBAL_SCORES_FILE
  if [ ! -s "$EXTRA_COMMAND_GLOBAL_SCORES_FILE" ] ; then echo "Fatal error: extra command ($EXTRA_COMMAND) scores file is empty" 1>&2 ; exit 1 ; fi
fi

##################################################
### Writing global results

echo target $TARGET_NAME > $SUMMARY_FILE
echo model $MODEL_NAME >> $SUMMARY_FILE
cat $CAD_SIZE_SCORES_FILE >> $SUMMARY_FILE
cat $CAD_GLOBAL_SCORES_FILE | grep -v "_diff" | grep -v "_ref" | grep -v "W" >> $SUMMARY_FILE
if $USE_ATOMIC_CADSCORE ; then cat $CAD_ATOMIC_GLOBAL_SCORES_FILE >> $SUMMARY_FILE ; fi
if $USE_TMSCORE ; then cat $TMSCORE_GLOBAL_SCORES_FILE >> $SUMMARY_FILE ; fi
if [ -n "$EXTRA_COMMAND" ] ; then cat $EXTRA_COMMAND_GLOBAL_SCORES_FILE >> $SUMMARY_FILE ; fi
