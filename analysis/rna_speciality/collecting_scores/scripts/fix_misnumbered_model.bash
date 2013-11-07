#!/bin/bash

CADSCORE_BIN_DIR=$1
GEOMETRIC_CHAINS_SPLITTING=$2
OUTPUT_DIR=$3
TARGET_FILE=$4
MODEL_FILE=$5

TARGET_BASENAME=$(basename $TARGET_FILE)
MODEL_BASENAME=$(basename $MODEL_FILE)

mkdir -p $OUTPUT_DIR/results
mkdir -p $OUTPUT_DIR/logs

exec > $OUTPUT_DIR/results/$MODEL_BASENAME
exec 2> $OUTPUT_DIR/logs/$MODEL_BASENAME

echo "Target file name:  " $(basename $TARGET_FILE) 1>&2
echo "Model file name:   " $(basename $MODEL_FILE) 1>&2
echo "" 1>&2

TMP_DIR=$(mktemp -d)

LOCAL_TARGET_FILE="$TMP_DIR/target"
cp $TARGET_FILE $LOCAL_TARGET_FILE

SPLIT_MODEL_FILE="$TMP_DIR/split_model"
if [ "$GEOMETRIC_CHAINS_SPLITTING" == "yes" ]
then
  $CADSCORE_BIN_DIR/voroprot2 --mode x-print-topological-ordering-of-residues --bond-distance 2 --print-ordered-pdb-file < $MODEL_FILE > $SPLIT_MODEL_FILE
else
  cp $MODEL_FILE $SPLIT_MODEL_FILE
fi

RENUMBERED_MODEL_FILE="$TMP_DIR/renumbered_model"
(cat $LOCAL_TARGET_FILE | $CADSCORE_BIN_DIR/voroprot2 --mode collect-atoms ; cat $SPLIT_MODEL_FILE | $CADSCORE_BIN_DIR/voroprot2 --mode collect-atoms) | $CADSCORE_BIN_DIR/voroprot2 --mode x-renumber-residues-by-reference --match 10 --mismatch -5 --gap-start -10 --gap-extension 0 --output-in-pdb-format --print-summary-log > $RENUMBERED_MODEL_FILE

$CADSCORE_BIN_DIR/CADscore_calc.bash -D $TMP_DIR/db -t $LOCAL_TARGET_FILE -m $RENUMBERED_MODEL_FILE -n -u -q -x 2> /dev/null
COMBINED_INTER_RESIDUE_CONTACTS_FILE="$TMP_DIR/db/targets/target/models/renumbered_model/combined_inter_residue_contacts"

FINAL_MODEL_FILE="$TMP_DIR/final_model"
if grep "# Renamed chains from" $COMBINED_INTER_RESIDUE_CONTACTS_FILE &> /dev/null
then
  CHAINS_BEFORE=$(grep "# Renamed chains from" $COMBINED_INTER_RESIDUE_CONTACTS_FILE | sed 's/# Renamed chains from ( \(.*\) ) to ( \(.*\) )/\1/g' | tr ' ' ',')
  CHAINS_AFTER=$(grep "# Renamed chains from" $COMBINED_INTER_RESIDUE_CONTACTS_FILE | sed 's/# Renamed chains from ( \(.*\) ) to ( \(.*\) )/\2/g' | tr ' ' ',')
  $CADSCORE_BIN_DIR/voroprot2 --mode x-rename-chains --before $CHAINS_BEFORE --after $CHAINS_AFTER < $RENUMBERED_MODEL_FILE > $FINAL_MODEL_FILE
else
  cp $RENUMBERED_MODEL_FILE $FINAL_MODEL_FILE
fi

echo "" 1>&2

cat $FINAL_MODEL_FILE

rm -r $TMP_DIR
