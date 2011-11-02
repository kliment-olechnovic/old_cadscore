#!/bin/bash

TARGET_FILE=$1
TARGET_NAME=$(basename $TARGET_FILE)

MODEL_FILE=$2
MODEL_NAME=$(basename $MODEL_FILE)

BOTH_NAMES=$TARGET_NAME"_and_"$MODEL_NAME

PARENT_OUTPUT_DIRECTORY=$3/$TARGET_NAME
OUTPUT_DIRECTORY=$PARENT_OUTPUT_DIRECTORY/$MODEL_NAME

SCORING_MODES=(0 1 2)
CONTACT_CATEGORIES=(AA AS SA SS)

mkdir -p $OUTPUT_DIRECTORY

TARGET_ALL_ATOMS_FILE=$PARENT_OUTPUT_DIRECTORY/all_atoms_of_target_$TARGET_NAME
if [ ! -f $TARGET_ALL_ATOMS_FILE ];
then
  cat $TARGET_FILE | ./voroprot2 --mode collect-atoms --radius-classes ./resources/vdwr_classes.txt --radius-members ./resources/vdwr_members.txt --include-heteroatoms 0 --include-water 0 > $TARGET_ALL_ATOMS_FILE
fi

TARGET_RESIDUE_IDS_FILE=$PARENT_OUTPUT_DIRECTORY/residue_ids_of_target_$TARGET_NAME
if [ ! -f $TARGET_RESIDUE_IDS_FILE ];
then
  cat $TARGET_ALL_ATOMS_FILE | ./voroprot2 --mode collect-residue-ids  > $TARGET_RESIDUE_IDS_FILE
fi

TARGET_INTER_ATOM_CONTACTS_FILE=$PARENT_OUTPUT_DIRECTORY/inter_atom_contacts_of_target_$TARGET_NAME
if [ ! -f $TARGET_INTER_ATOM_CONTACTS_FILE ];
then
  cat $TARGET_ALL_ATOMS_FILE | timeout 30s ./voroprot2 --mode construct-inter-atom-contacts --depth 3 --probe 1.4 > $TARGET_INTER_ATOM_CONTACTS_FILE
fi

MODEL_ALL_ATOMS_FILE=$OUTPUT_DIRECTORY/all_atoms_of_model_$MODEL_NAME
cat $MODEL_FILE | ./voroprot2 --mode collect-atoms --radius-classes ./resources/vdwr_classes.txt --radius-members ./resources/vdwr_members.txt --include-heteroatoms 0 --include-water 0 > $MODEL_ALL_ATOMS_FILE

MODEL_SELECTED_ATOMS_FILE=$OUTPUT_DIRECTORY/selected_atoms_of_model_$MODEL_NAME
cat $TARGET_ALL_ATOMS_FILE $MODEL_ALL_ATOMS_FILE | ./voroprot2 --mode filter-atoms-by-target > $MODEL_SELECTED_ATOMS_FILE

MODEL_RESIDUE_IDS_FILE=$OUTPUT_DIRECTORY/residue_ids_of_model_$MODEL_NAME
cat $MODEL_SELECTED_ATOMS_FILE | ./voroprot2 --mode collect-residue-ids  > $MODEL_RESIDUE_IDS_FILE

MODEL_INTER_ATOM_CONTACTS_FILE=$OUTPUT_DIRECTORY/inter_atom_contacts_of_model_$MODEL_NAME
cat $MODEL_SELECTED_ATOMS_FILE | timeout 30s ./voroprot2 --mode construct-inter-atom-contacts --depth 3 --probe 1.4 > $MODEL_INTER_ATOM_CONTACTS_FILE

for SCORING_MODE in ${SCORING_MODES[*]}
do
  CAD_PROFILE_FILE=$OUTPUT_DIRECTORY/cad_profile_$SCORING_MODE"_"of_$BOTH_NAMES
  cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | ./voroprot2 --mode calculate-contact-area-difference-profile --intersect-sequences 0 --scoring-mode $SCORING_MODE > $CAD_PROFILE_FILE

  GLOBAL_SCORES_FILE=$OUTPUT_DIRECTORY/global_scores_$SCORING_MODE"_"of_$BOTH_NAMES
  cat $CAD_PROFILE_FILE | ./voroprot2 --mode calculate-contact-area-difference-global-score --use-min 1 --prefix sm$SCORING_MODE > $GLOBAL_SCORES_FILE

  for CONTACT_CATEGORY in ${CONTACT_CATEGORIES[*]}
  do
    LOCAL_SCORES_PLOT_FILE=$OUTPUT_DIRECTORY/local_scores_plot_$SCORING_MODE"_"$CONTACT_CATEGORY"_"of_$BOTH_NAMES.ppm
    cat $CAD_PROFILE_FILE | ./voroprot2 --mode print-contact-area-difference-local-scores-plot --category $CONTACT_CATEGORY --max-window 30 > $LOCAL_SCORES_PLOT_FILE
  done
done

COMBINED_INTER_RESIDUE_CONTACTS_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts_of_$BOTH_NAMES
cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | ./voroprot2 --mode combine-inter-residue-contacts > $COMBINED_INTER_RESIDUE_CONTACTS_FILE

for CONTACT_CATEGORY in ${CONTACT_CATEGORIES[*]}
do
  COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PPM=$OUTPUT_DIRECTORY/combined_inter_residue_contacts_plot_$CONTACT_CATEGORY"_"of_$BOTH_NAMES.ppm
  cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE | ./voroprot2 --mode print-combined-inter-residue-contacts-plot --category $CONTACT_CATEGORY > $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PPM
done

DESCRIPTION_FILE=$OUTPUT_DIRECTORY/description_of_$BOTH_NAMES
echo "target $TARGET_NAME" > $DESCRIPTION_FILE
echo "model $MODEL_NAME" >> $DESCRIPTION_FILE
echo target_atoms_count `sed -n '2p' $TARGET_ALL_ATOMS_FILE` >> $DESCRIPTION_FILE
echo model_atoms_count `sed -n '2p' $MODEL_SELECTED_ATOMS_FILE` >> $DESCRIPTION_FILE
echo target_residues_count `sed -n '2p' $TARGET_RESIDUE_IDS_FILE` >> $DESCRIPTION_FILE
echo model_residues_count `sed -n '2p' $MODEL_RESIDUE_IDS_FILE` >> $DESCRIPTION_FILE

TMSCORE_PROFILE_FILE=$OUTPUT_DIRECTORY/tm_score_profile_of_$BOTH_NAMES
TMscore $MODEL_FILE $TARGET_FILE > $TMSCORE_PROFILE_FILE
if grep -q "TM-score" $TMSCORE_PROFILE_FILE
then
  TMSCORE_RESULTS_FILE=$OUTPUT_DIRECTORY/tm_score_results_of_$BOTH_NAMES
  echo TM_score `cat $TMSCORE_PROFILE_FILE | egrep "TM-score\s*=.*d0" | sed 's/TM-score\s*=\s*\(.*\)\s*(.*/\1/g'` > $TMSCORE_RESULTS_FILE
  echo TM_score_GDT_TS `cat $TMSCORE_PROFILE_FILE | egrep "GDT-TS-score" | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g'` >> $TMSCORE_RESULTS_FILE
  echo TM_score_GDT_HA `cat $TMSCORE_PROFILE_FILE | egrep "GDT-HA-score" | sed 's/GDT-HA-score\s*=\s*\(.*\)\s*%(d<0\.5).*/\1/g'` >> $TMSCORE_RESULTS_FILE
fi

SUMMARY_FILE=$OUTPUT_DIRECTORY/summary_of_$BOTH_NAMES
cat $DESCRIPTION_FILE $OUTPUT_DIRECTORY/global_scores_* $OUTPUT_DIRECTORY/tm_score_results_* > $SUMMARY_FILE

TABLE_SUMMARY_FILE=$OUTPUT_DIRECTORY/table_summary_of_$BOTH_NAMES
for i in 1 2 ; do cat $SUMMARY_FILE | cut --delimiter " " --fields $i | paste -s ; done > $TABLE_SUMMARY_FILE
