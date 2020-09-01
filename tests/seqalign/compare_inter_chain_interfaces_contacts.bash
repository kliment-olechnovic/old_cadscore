#!/bin/bash

usage="Calculate CAD-score for two contacts files.

Command line options:
-1: Target contacts file;
-2: Model contacts file;
-i: take only inter-chain contacts;
-I: take contacts between chains A and B (inter-chain option for clustering 
    of standard files with ligands);
-b: use binarized CAD-score (Fraction of Common Contacts);
-v: voroprot2 command (default: voroprot2);
-O: calculate one-sided CAD-score (only exposed value will be reliable);
-M: main chain for one-sided CAD-score calculation is B (default A);
-a: use sequence alignments;
-s: swap model chains;
-r: try to optimally rename chains.
"

if [[ $# -eq 0 ]];
then
    echo "$usage"
    exit 1
fi

VOROPROT='voroprot2'
INTER_CHAIN=''
OPTIMALLY_RENAME_CHAINS=''
BINARIZE=''
USE_SEQUENCE_ALIGNMENTS=false
SWAP_MODEL_CHAINS=false
ONE_SIDE_SCORE=false
MAIN_CHAIN_IS_A=true

while getopts '1:2:iIbv:asOMhr' opt; do
    case $opt in
        1) TARGET_CONTACTS_FILE="$OPTARG";;
        2) MODEL_CONTACTS_FILE="$OPTARG";;
        i) INTER_CHAIN="--inter-chain";;
        I) INTER_CHAIN="--inter-interval '(A)(B)'";;
        b) BINARIZE="--binarize";;
        v) VOROPROT="$OPTARG";;
        a) USE_SEQUENCE_ALIGNMENTS=true;;
        r) OPTIMALLY_RENAME_CHAINS='--optimally-rename-chains';;
        s) SWAP_MODEL_CHAINS=true;;
        O) ONE_SIDE_SCORE=true;;
        M) MAIN_CHAIN_IS_A=false;;
        h) echo "$usage"; exit 0;;
    esac
done

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

if [ -d /dev/shm ]; then
    tmp_prefix="/dev/shm"
elif [ -d /run/shm ]; then
    tmp_prefix="/run/shm"
else
    tmp_prefix="/tmp"
fi

target_contacts_temporary_file=$(mktemp -p $tmp_prefix)
model_contacts_temporary_file=$(mktemp -p $tmp_prefix)

# If using sequence alignments, then they are taken from stdin and contacts files are renamed according to them.
if $USE_SEQUENCE_ALIGNMENTS; then
    alignments=''
    while read -r line; do
        alignments+="$line\n"
    done
    all_contacts_temporary_file=$(mktemp -p $tmp_prefix)

    if $ONE_SIDE_SCORE; then
        if $MAIN_CHAIN_IS_A; then
            chains=('A' 'B')
        else
            chains=('B' 'A')
        fi
        target_chain=${chains[0]}
        if $SWAP_MODEL_CHAINS; then
            model_chain=${chains[1]}
        else
            model_chain=${chains[0]}
        fi
        TARGET_CHAIN_NAMES="--target-chain-names $target_chain"
        MODEL_CHAIN_NAMES="--model-chain-names $model_chain"
    else
        TARGET_CHAIN_NAMES="--target-chain-names AB"
        if $SWAP_MODEL_CHAINS; then
            MODEL_CHAIN_NAMES="--model-chain-names BA"
        else
            MODEL_CHAIN_NAMES="--model-chain-names AB"
        fi
    fi
    (cat $TARGET_CONTACTS_FILE $MODEL_CONTACTS_FILE; echo -e "$alignments") | \
        $VOROPROT --mode x-renumber-residues-in-inter-atom-contacts $TARGET_CHAIN_NAMES $MODEL_CHAIN_NAMES > $all_contacts_temporary_file

    # Target and model contacts had to be joined for renaming of chains.
    # After that, the combined contacts file is splitted back to target and 
    # model contacts, now renamed.
    # sed is used for splitting, according to:
    # http://biounix.blogspot.com/2009/04/split-file-in-2-with-sed.html
    #
    # Because of such problems with text manipulation, this script should be re-written in python :-)
    (cat $all_contacts_temporary_file | sed '1,/^atoms/!d' | sed '$d') > $target_contacts_temporary_file
    (echo "atoms"; cat $all_contacts_temporary_file | sed '1,/^atoms/d') > $model_contacts_temporary_file
    rm $all_contacts_temporary_file

    TARGET_CONTACTS_FILE=$target_contacts_temporary_file
    MODEL_CONTACTS_FILE=$model_contacts_temporary_file
fi
if $SWAP_MODEL_CHAINS; then
    # Renaming chains in model contacts file.
    if $USE_SEQUENCE_ALIGNMENTS; then
        # Model contacts file is a temporary file, we can modify it
        # directly.
        sed -i 's/^A/#/g; s/^B/A/g; s/^#/B/g' $MODEL_CONTACTS_FILE
    else
        # We cannot change the original input file, therefore we do
        # modifications in a temporary file.
        sed 's/^A/#/g; s/^B/A/g; s/^#/B/g' $MODEL_CONTACTS_FILE > $model_contacts_temporary_file
        MODEL_CONTACTS_FILE=$model_contacts_temporary_file
    fi
fi

# Saving intermediate calculations into temporary files
target_inter_residue_contacts=$(mktemp -p $tmp_prefix)
model_inter_residue_contacts=$(mktemp -p $tmp_prefix)
target_residue_ids=$(mktemp -p $tmp_prefix)

cat $TARGET_CONTACTS_FILE | $VOROPROT --mode calc-inter-residue-contacts $INTER_CHAIN | \
    $VOROPROT --mode x-categorize-residue-interface-exposure > $target_inter_residue_contacts
cat $MODEL_CONTACTS_FILE | $VOROPROT --mode calc-inter-residue-contacts $INTER_CHAIN | \
    $VOROPROT --mode x-categorize-residue-interface-exposure > $model_inter_residue_contacts
cat $TARGET_CONTACTS_FILE | $VOROPROT --mode collect-residue-ids > $target_residue_ids

# Final CAD-score calculation
combined_inter_residue_contacts=$(mktemp -p $tmp_prefix)
( \
    ( \
        cat $target_inter_residue_contacts; \
        cat $model_inter_residue_contacts; \
        cat $target_residue_ids \
    ) \
    | $VOROPROT --mode calc-combined-inter-residue-contacts $OPTIMALLY_RENAME_CHAINS $BINARIZE; \
    cat $target_residue_ids \
) > $combined_inter_residue_contacts

function calculate_CAD_score()
{
    local combined_contacts="$1"
    local cadscore_type="$2"

    cat $combined_contacts \
    | $VOROPROT --mode calc-CAD-profile $cadscore_type \
    | $VOROPROT --mode calc-CAD-global-scores --add-categories AA_exposed,AA_exposed_to_B,AA_exposed_to_A \
    | grep -v '_ref' | grep -v '_diff' | grep 'AA'
}

calculate_CAD_score $combined_inter_residue_contacts
# type 3 is calculation of total area score (intersecting/interface_area)
calculate_CAD_score $combined_inter_residue_contacts "--type 3" | sed s/^/area_/

# Removing temporary files
rm $target_contacts_temporary_file
rm $model_contacts_temporary_file
rm $target_inter_residue_contacts
rm $model_inter_residue_contacts
rm $target_residue_ids
rm $combined_inter_residue_contacts
