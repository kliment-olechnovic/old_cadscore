#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -t    path to target file in PBD format
  -m    path to model file in PBD format
  -l    flag to include heteroatoms (optional)
  -f    flag to force one chain name (optional)
  -c    flag to consider only inter-chain contacts (optional)
  -i    inter-interval contacts specification (optional)
  -o    max timeout (optional)

EOF
}

SCRIPT_DIRECTORY=$(dirname $0)
VOROPROT="$SCRIPT_DIRECTORY/voroprot2"
TMSCORE_CALC="$SCRIPT_DIRECTORY/TMscore_calc.bash"

if [ ! -f "$VOROPROT" ]
then
  echo "$VOROPROT does not exist" 1>&2
  exit 1
fi

if [ ! -f "$TMSCORE_CALC" ]
then
  echo "$TMSCORE_CALC does not exist" 1>&2
  exit 1
fi

DATABASE=""
TARGET_FILE=""
MODEL_FILE=""
HETATM_FLAG=""
FORCE_ONE_CHAIN_FLAG=""
INTER_CHAIN_FLAG=""
INTER_INTERVAL_OPTION=""
TIMEOUT="300s"

while getopts "hD:t:m:lfci:o:" OPTION
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
    f)
      FORCE_ONE_CHAIN_FLAG="--force-one-chain"
      ;;
    c)
      INTER_CHAIN_FLAG="--inter-chain"
      ;;
    i)
      INTER_INTERVAL_OPTION='--inter-interval "'$OPTARG'"'
      ;;
    o)
      TIMEOUT=$OPTARG
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
  echo "Target file \"$TARGET_FILE\" does not exist" 1>&2
  exit 1
fi

if [ -f "$MODEL_FILE" ]
then
  MODEL_NAME=$(basename $MODEL_FILE)
else
  echo "Model file \"$MODEL_FILE\" does not exist" 1>&2
  exit 1
fi

TARGET_DIR="$DATABASE/$TARGET_NAME"
TARGET_ATOMS_FILE="$TARGET_DIR/atoms"
TARGET_RESIDUE_IDS_FILE="$TARGET_DIR/residue_ids"
TARGET_INTER_ATOM_CONTACTS_FILE="$TARGET_DIR/inter_atom_contacts"

MODEL_DIR="$DATABASE/$TARGET_NAME/$MODEL_NAME"
MODEL_ATOMS_FILE="$MODEL_DIR/atoms"
MODEL_RESIDUE_IDS_FILE="$MODEL_DIR/residue_ids"
MODEL_INTER_ATOM_CONTACTS_FILE="$MODEL_DIR/inter_atom_contacts"

COMBINED_INTER_RESIDUE_CONTACTS_FILE="$MODEL_DIR/combined_inter_residue_contacts"
CAD_PROFILE_FILE="$MODEL_DIR/cad_profile"
CAD_GLOBAL_SCORES_FILE="$MODEL_DIR/cad_global_scores"

TMSCORE_PROFILE_FILE="$MODEL_DIR/tmscore_profile"
TMSCORE_GLOBAL_SCORES_FILE="$MODEL_DIR/tmscore_global_scores"

SUMMARY_FILE="$MODEL_DIR/summary"
LOGS_FILE="$MODEL_DIR/logs"

mkdir -p "$TARGET_DIR"
mkdir -p "$MODEL_DIR"

test -f $TARGET_ATOMS_FILE || cat $TARGET_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG $FORCE_ONE_CHAIN_FLAG > $TARGET_ATOMS_FILE 2> "$TARGET_ATOMS_FILE.log"
test -f $TARGET_RESIDUE_IDS_FILE || cat $TARGET_ATOMS_FILE | $VOROPROT --mode collect-residue-ids  > $TARGET_RESIDUE_IDS_FILE 2> "$TARGET_RESIDUE_IDS_FILE.log"
test -f $TARGET_INTER_ATOM_CONTACTS_FILE || cat $TARGET_ATOMS_FILE | timeout $TIMEOUT $VOROPROT --mode calc-inter-atom-contacts > $TARGET_INTER_ATOM_CONTACTS_FILE 2> "$TARGET_INTER_ATOM_CONTACTS_FILE.log"

if [ ! -f "$MODEL_ATOMS_FILE" ]
then
  cat $MODEL_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG $FORCE_ONE_CHAIN_FLAG > $MODEL_ATOMS_FILE 2> "$MODEL_ATOMS_FILE.log"
  cat $MODEL_ATOMS_FILE $TARGET_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode filter-atoms-by-target > "$MODEL_ATOMS_FILE.filtered" 2>> "$MODEL_ATOMS_FILE.log"
  mv "$MODEL_ATOMS_FILE.filtered" $MODEL_ATOMS_FILE
fi
test -f $MODEL_RESIDUE_IDS_FILE || cat $MODEL_ATOMS_FILE | $VOROPROT --mode collect-residue-ids  > $MODEL_RESIDUE_IDS_FILE 2> "$MODEL_RESIDUE_IDS_FILE.log"
test -f $MODEL_INTER_ATOM_CONTACTS_FILE || cat $MODEL_ATOMS_FILE | timeout $TIMEOUT $VOROPROT --mode calc-inter-atom-contacts > $MODEL_INTER_ATOM_CONTACTS_FILE 2> "$MODEL_INTER_ATOM_CONTACTS_FILE.log"

test -f $COMBINED_INTER_RESIDUE_CONTACTS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-combined-inter-residue-contacts $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION > $COMBINED_INTER_RESIDUE_CONTACTS_FILE 2> "$COMBINED_INTER_RESIDUE_CONTACTS_FILE.log"
test -f $CAD_PROFILE_FILE || cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE $TARGET_RESIDUE_IDS_FILE | $VOROPROT --mode calc-CAD-profile > $CAD_PROFILE_FILE 2> "$CAD_PROFILE_FILE.log"
test -f $CAD_GLOBAL_SCORES_FILE || cat $CAD_PROFILE_FILE | $VOROPROT --mode calc-CAD-global-scores > $CAD_GLOBAL_SCORES_FILE 2> "$CAD_GLOBAL_SCORES_FILE.log"

test -f $TMSCORE_GLOBAL_SCORES_FILE || $TMSCORE_CALC -m $MODEL_FILE -t $TARGET_FILE -p $TMSCORE_PROFILE_FILE -s $TMSCORE_GLOBAL_SCORES_FILE

if [ -s "$CAD_GLOBAL_SCORES_FILE" ] && [ -s "$TMSCORE_GLOBAL_SCORES_FILE" ] && [ -s "$TARGET_ATOMS_FILE" ] && [ -s "$MODEL_ATOMS_FILE" ] && [ -s "$TARGET_RESIDUE_IDS_FILE" ] && [ -s "$MODEL_RESIDUE_IDS_FILE" ]
then
  echo target $TARGET_NAME > $SUMMARY_FILE
  echo model $MODEL_NAME >> $SUMMARY_FILE
  echo target_atoms_count `sed -n '2p' $TARGET_ATOMS_FILE` >> $SUMMARY_FILE
  echo model_atoms_count `sed -n '2p' $MODEL_ATOMS_FILE` >> $SUMMARY_FILE
  echo target_residues_count `sed -n '2p' $TARGET_RESIDUE_IDS_FILE` >> $SUMMARY_FILE
  echo model_residues_count `sed -n '2p' $MODEL_RESIDUE_IDS_FILE` >> $SUMMARY_FILE
  cat $CAD_GLOBAL_SCORES_FILE | grep -v "_diff" | grep -v "_ref" >> $SUMMARY_FILE
  cat $TMSCORE_GLOBAL_SCORES_FILE >> $SUMMARY_FILE
else
  if [ -f "$SUMMARY_FILE" ]
  then
    rm $SUMMARY_FILE
  fi
fi

cat "$TARGET_ATOMS_FILE.log" "$TARGET_RESIDUE_IDS_FILE.log" "$TARGET_INTER_ATOM_CONTACTS_FILE.log" > $LOGS_FILE
cat "$MODEL_ATOMS_FILE.log" "$MODEL_RESIDUE_IDS_FILE.log" "$MODEL_INTER_ATOM_CONTACTS_FILE.log" >> $LOGS_FILE
cat "$COMBINED_INTER_RESIDUE_CONTACTS_FILE.log" "$CAD_PROFILE_FILE.log" "$CAD_GLOBAL_SCORES_FILE.log" >> $LOGS_FILE
cat "$TMSCORE_PROFILE_FILE.log" >> $LOGS_FILE
