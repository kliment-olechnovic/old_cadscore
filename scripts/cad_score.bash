#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)
VOROPROT=$SCRIPT_DIRECTORY/voroprot2

###########################################

INCLUDE_HETEROATOMS="0"
FILTER_MODEL_ATOMS_BY_TARGET=true
SUBDIVISION_DEPTH="3"
PROBE_RADIUS="1.4"
CALCULATION_TIMEOUT="300s"
CHAIN_MODE="1"
SCORING_MODES=(0 1 2)
LOCAL_SCORES_CONTACT_CATEGORIES=()
LOCAL_SCORES_WINDOWS=()
LOCAL_SCORES_PLOTS_CONTACT_CATEGORIES=()
LOCAL_SCORES_PLOTS_MAX_WINDOW="10"
LOCAL_SCORES_INJECTION_CONTACT_CATEGORIES=()
LOCAL_SCORES_INJECTION_WINDOWS=()
COMBINED_INTER_RESIDUE_CONTACT_PLOTS_CATEGORIES=()
USE_TM_SCORE=true

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -O    path to writable output directory
  -t    path to target file in PBD format
  -m    path to model file in PBD format
  -f    path to filter file (optional)

EOF
}

while getopts “hO:t:m:f:” OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    O)
      OUTPUT_PATH=$OPTARG
      ;;
    t)
      TARGET_FILE=$OPTARG
      ;;
    m)
      MODEL_FILE=$OPTARG
      ;;
    f)
      FILTER_FILE=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$OUTPUT_PATH" ] || [ -z "$TARGET_FILE" ] || [ -z "$MODEL_FILE" ]
then
  print_help
  exit 1
fi

DOMAIN_DIRECTORY=$OUTPUT_PATH"/cad_score"

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

if [ -n "$FILTER_FILE" ] && [ -f "$FILTER_FILE" ]
then
  FILTER_NAME=$(basename $FILTER_FILE)
fi

###########################################

TARGET_CONTACTS_DIRECTORY=$DOMAIN_DIRECTORY"/contacts"/$TARGET_NAME
mkdir -p $TARGET_CONTACTS_DIRECTORY

TARGET_ALL_ATOMS_FILE=$TARGET_CONTACTS_DIRECTORY/all_atoms
test -f $TARGET_ALL_ATOMS_FILE || cat $TARGET_FILE | $VOROPROT --mode collect-atoms --include-heteroatoms $INCLUDE_HETEROATOMS --include-water 0 --force-one-chain $CHAIN_MODE > $TARGET_ALL_ATOMS_FILE 2> $TARGET_ALL_ATOMS_FILE.log

TARGET_RESIDUE_IDS_FILE=$TARGET_CONTACTS_DIRECTORY/residue_ids
test -f $TARGET_RESIDUE_IDS_FILE || cat $TARGET_ALL_ATOMS_FILE | $VOROPROT --mode collect-residue-ids  > $TARGET_RESIDUE_IDS_FILE 2> $TARGET_RESIDUE_IDS_FILE.log

TARGET_INTER_ATOM_CONTACTS_FILE=$TARGET_CONTACTS_DIRECTORY/inter_atom_contacts
test -f $TARGET_INTER_ATOM_CONTACTS_FILE || cat $TARGET_ALL_ATOMS_FILE | timeout $CALCULATION_TIMEOUT $VOROPROT --mode construct-inter-atom-contacts --depth $SUBDIVISION_DEPTH --probe $PROBE_RADIUS > $TARGET_INTER_ATOM_CONTACTS_FILE 2> $TARGET_INTER_ATOM_CONTACTS_FILE.log

###################

MODEL_CONTACTS_DIRECTORY=$DOMAIN_DIRECTORY"/contacts"/$MODEL_NAME
mkdir -p $MODEL_CONTACTS_DIRECTORY

MODEL_ALL_ATOMS_FILE=$MODEL_CONTACTS_DIRECTORY/all_atoms
test -f $MODEL_ALL_ATOMS_FILE || cat $MODEL_FILE | $VOROPROT --mode collect-atoms --include-heteroatoms $INCLUDE_HETEROATOMS --include-water 0 --force-one-chain $CHAIN_MODE > $MODEL_ALL_ATOMS_FILE 2> $MODEL_ALL_ATOMS_FILE.log

if $FILTER_MODEL_ATOMS_BY_TARGET
then
  MODEL_CONTACTS_DIRECTORY=$MODEL_CONTACTS_DIRECTORY"/"$TARGET_NAME
  mkdir -p $MODEL_CONTACTS_DIRECTORY
  
  MODEL_SELECTED_ATOMS_FILE=$MODEL_CONTACTS_DIRECTORY/selected_atoms
  test -f $MODEL_SELECTED_ATOMS_FILE || cat $TARGET_ALL_ATOMS_FILE $MODEL_ALL_ATOMS_FILE | $VOROPROT --mode filter-atoms-by-target > $MODEL_SELECTED_ATOMS_FILE 2> $MODEL_SELECTED_ATOMS_FILE.log
  
  MODEL_ALL_ATOMS_FILE=$MODEL_SELECTED_ATOMS_FILE
fi

MODEL_RESIDUE_IDS_FILE=$MODEL_CONTACTS_DIRECTORY/residue_ids
test -f $MODEL_RESIDUE_IDS_FILE || cat $MODEL_ALL_ATOMS_FILE | $VOROPROT --mode collect-residue-ids  > $MODEL_RESIDUE_IDS_FILE 2> $MODEL_RESIDUE_IDS_FILE.log

MODEL_INTER_ATOM_CONTACTS_FILE=$MODEL_CONTACTS_DIRECTORY/inter_atom_contacts
test -f $MODEL_INTER_ATOM_CONTACTS_FILE || cat $MODEL_ALL_ATOMS_FILE | timeout $CALCULATION_TIMEOUT $VOROPROT --mode construct-inter-atom-contacts --depth $SUBDIVISION_DEPTH --probe $PROBE_RADIUS > $MODEL_INTER_ATOM_CONTACTS_FILE 2> $MODEL_INTER_ATOM_CONTACTS_FILE.log

###################

OUTPUT_DIRECTORY=$DOMAIN_DIRECTORY"/comparison/"$TARGET_NAME"/"$MODEL_NAME
mkdir -p $OUTPUT_DIRECTORY

COMBINED_INTER_RESIDUE_CONTACTS_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts
test -f $COMBINED_INTER_RESIDUE_CONTACTS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode combine-inter-residue-contacts > $COMBINED_INTER_RESIDUE_CONTACTS_FILE 2> $COMBINED_INTER_RESIDUE_CONTACTS_FILE.log

COMBINED_INTER_RESIDUE_CONTACTS_SEQUENCE_MAP_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts_sequence_map
test -f $COMBINED_INTER_RESIDUE_CONTACTS_SEQUENCE_MAP_FILE || cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE | $VOROPROT --mode print-combined-inter-residue-contacts-sequence-map > $COMBINED_INTER_RESIDUE_CONTACTS_SEQUENCE_MAP_FILE 2> $COMBINED_INTER_RESIDUE_CONTACTS_SEQUENCE_MAP_FILE.log

if [ -n "$FILTER_NAME" ]
then
  TARGET_NAME=$TARGET_NAME"_filtered_by_"$FILTER_NAME
  OUTPUT_DIRECTORY=$DOMAIN_DIRECTORY"/comparison/"$TARGET_NAME"/"$MODEL_NAME
  mkdir -p $OUTPUT_DIRECTORY
  
  FULL_COMBINED_INTER_RESIDUE_CONTACTS_FILE=$COMBINED_INTER_RESIDUE_CONTACTS_FILE
  COMBINED_INTER_RESIDUE_CONTACTS_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts
  test -f $COMBINED_INTER_RESIDUE_CONTACTS_FILE || cat $FULL_COMBINED_INTER_RESIDUE_CONTACTS_FILE $FILTER_FILE | $VOROPROT --mode filter-combined-inter-residue-contacts --filter-mode 0 > $COMBINED_INTER_RESIDUE_CONTACTS_FILE 2> $COMBINED_INTER_RESIDUE_CONTACTS_FILE.log

  COMBINED_INTER_RESIDUE_CONTACTS_SEQUENCE_MAP_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts_sequence_map
  test -f $COMBINED_INTER_RESIDUE_CONTACTS_SEQUENCE_MAP_FILE || cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE | $VOROPROT --mode print-combined-inter-residue-contacts-sequence-map > $COMBINED_INTER_RESIDUE_CONTACTS_SEQUENCE_MAP_FILE 2> $COMBINED_INTER_RESIDUE_CONTACTS_SEQUENCE_MAP_FILE.log
fi

for SCORING_MODE in ${SCORING_MODES[*]}
do
  CAD_PROFILE_FILE=$OUTPUT_DIRECTORY/cad_profile_$SCORING_MODE
  test -f $CAD_PROFILE_FILE || cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE $TARGET_RESIDUE_IDS_FILE | $VOROPROT --mode calculate-contact-area-difference-profile --scoring-mode $SCORING_MODE > $CAD_PROFILE_FILE 2> $CAD_PROFILE_FILE.log

  GLOBAL_SCORES_FILE=$OUTPUT_DIRECTORY/global_scores_$SCORING_MODE
  test -f $GLOBAL_SCORES_FILE || cat $CAD_PROFILE_FILE | $VOROPROT --mode calculate-contact-area-difference-global-score --use-min 1 --prefix sm$SCORING_MODE > $GLOBAL_SCORES_FILE 2> $GLOBAL_SCORES_FILE.log

  for CONTACT_CATEGORY in ${LOCAL_SCORES_CONTACT_CATEGORIES[*]}
  do
    for WINDOW in ${LOCAL_SCORES_WINDOWS[*]}
    do
  	  LOCAL_SCORES_FILE=$OUTPUT_DIRECTORY/local_scores/$SCORING_MODE"_"$CONTACT_CATEGORY"_"$WINDOW
      if [ ! -f $LOCAL_SCORES_FILE ]
      then
  	    mkdir -p $OUTPUT_DIRECTORY/local_scores/
        cat $CAD_PROFILE_FILE | $VOROPROT --mode calculate-contact-area-difference-local-scores --category $CONTACT_CATEGORY --window $WINDOW > $LOCAL_SCORES_FILE 2> $LOCAL_SCORES_FILE.log
      fi
    done
  done

  for CONTACT_CATEGORY in ${LOCAL_SCORES_PLOTS_CONTACT_CATEGORIES[*]}
  do
  	LOCAL_SCORES_PLOT_FILE=$OUTPUT_DIRECTORY/local_scores_plot/$SCORING_MODE"_"$CONTACT_CATEGORY"_"$LOCAL_SCORES_PLOTS_MAX_WINDOW.ppm
    if [ ! -f $LOCAL_SCORES_PLOT_FILE.png ]
    then
  	  mkdir -p $OUTPUT_DIRECTORY/local_scores_plot/
      cat $CAD_PROFILE_FILE | $VOROPROT --mode print-contact-area-difference-local-scores-plot --category $CONTACT_CATEGORY --max-window $LOCAL_SCORES_PLOTS_MAX_WINDOW > $LOCAL_SCORES_PLOT_FILE 2> $LOCAL_SCORES_PLOT_FILE.log
      convert $LOCAL_SCORES_PLOT_FILE $LOCAL_SCORES_PLOT_FILE.png 2> $LOCAL_SCORES_PLOT_FILE.png.log
      rm $LOCAL_SCORES_PLOT_FILE
    fi
  done
  
  for CONTACT_CATEGORY in ${LOCAL_SCORES_INJECTION_CONTACT_CATEGORIES[*]}
  do
  	for INJECTION_WINDOW in ${LOCAL_SCORES_INJECTION_WINDOWS[*]}
  	do
  	  LOCAL_SCORES_INJECTED_TO_MODEL_PDB_FILE=$OUTPUT_DIRECTORY/local_scores_injected_to_model/$SCORING_MODE"_"$CONTACT_CATEGORY"_"$INJECTION_WINDOW.pdb
      if [ ! -f $LOCAL_SCORES_INJECTED_TO_MODEL_PDB_FILE ]
      then
        mkdir -p $OUTPUT_DIRECTORY/local_scores_injected_to_model/
        (cat $CAD_PROFILE_FILE | $VOROPROT --mode calculate-contact-area-difference-local-scores --category $CONTACT_CATEGORY --window $INJECTION_WINDOW ; cat $MODEL_FILE) | $VOROPROT --mode print-contact-area-difference-local-scores-injected-to-pdb-file > $LOCAL_SCORES_INJECTED_TO_MODEL_PDB_FILE 2> $LOCAL_SCORES_INJECTED_TO_MODEL_PDB_FILE.log
      fi
    done
  done
done

###################

for CONTACT_CATEGORY in ${COMBINED_INTER_RESIDUE_CONTACT_PLOTS_CATEGORIES[*]}
do
  COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts_plot/$CONTACT_CATEGORY".ppm"
  if [ ! -f $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE.png ]
  then
    mkdir -p $OUTPUT_DIRECTORY/combined_inter_residue_contacts_plot/
    cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE | $VOROPROT --mode print-combined-inter-residue-contacts-plot --category $CONTACT_CATEGORY > $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE 2> $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE.log
    convert $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE.png
    rm $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE
  fi
done

###################

SUMMARY_LIST_FILE=$OUTPUT_DIRECTORY/summary_list
echo "target $TARGET_NAME" > $SUMMARY_LIST_FILE
echo "model $MODEL_NAME" >> $SUMMARY_LIST_FILE
echo target_atoms_count `sed -n '2p' $TARGET_ALL_ATOMS_FILE` >> $SUMMARY_LIST_FILE
echo model_atoms_count `sed -n '2p' $MODEL_ALL_ATOMS_FILE` >> $SUMMARY_LIST_FILE
echo target_residues_count `sed -n '2p' $TARGET_RESIDUE_IDS_FILE` >> $SUMMARY_LIST_FILE
echo model_residues_count `sed -n '2p' $MODEL_RESIDUE_IDS_FILE` >> $SUMMARY_LIST_FILE
for SCORING_MODE in ${SCORING_MODES[*]} ; do cat $OUTPUT_DIRECTORY/global_scores_$SCORING_MODE ; done >> $SUMMARY_LIST_FILE

if $USE_TM_SCORE
then
  TMSCORE_PROFILE_FILE=$OUTPUT_DIRECTORY/tm_score_profile
  test -f $TMSCORE_PROFILE_FILE || TMscore $MODEL_FILE $TARGET_FILE > $TMSCORE_PROFILE_FILE 2> $TMSCORE_PROFILE_FILE.log

  TM_SCORE=$(cat $TMSCORE_PROFILE_FILE | egrep "TM-score\s*=.*d0" | sed 's/TM-score\s*=\s*\(.*\)\s*(.*/\1/g')
  if [ -z "$TM_SCORE" ] ; then TM_SCORE=0 ; fi

  TM_SCORE_GDT_TS=$(cat $TMSCORE_PROFILE_FILE | egrep "GDT-TS-score" | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g')
  if [ -z "$TM_SCORE_GDT_TS" ] ; then TM_SCORE_GDT_TS=0 ; fi

  TM_SCORE_GDT_HA=$(cat $TMSCORE_PROFILE_FILE | egrep "GDT-HA-score" | sed 's/GDT-HA-score\s*=\s*\(.*\)\s*%(d<0\.5).*/\1/g')
  if [ -z "$TM_SCORE_GDT_HA" ] ; then TM_SCORE_GDT_HA=0 ; fi

  echo "TM_score $TM_SCORE" >> $SUMMARY_LIST_FILE
  echo "TM_score_GDT_TS $TM_SCORE_GDT_TS" >> $SUMMARY_LIST_FILE
  echo "TM_score_GDT_HA $TM_SCORE_GDT_HA" >> $SUMMARY_LIST_FILE
fi

SUMMARY_TABLE_FILE=$OUTPUT_DIRECTORY/summary_table
for i in 1 2 ; do cat $SUMMARY_LIST_FILE | cut --delimiter " " --fields $i | paste -s ; done > $SUMMARY_TABLE_FILE
