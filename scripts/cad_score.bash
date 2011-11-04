#!/bin/bash

INCLUDE_HETEROATOMS="0"
SUBDIVISION_DEPTH="3"
PROBE_RADIUS="1.4"
CALCULATION_TIMEOUT="3000s"
SCORING_MODES=(0 1 2)
LOCAL_SCORES_PLOTS_CONTACT_CATEGORIES=(AA)
LOCAL_SCORES_PLOTS_MAX_WINDOW="20"
LOCAL_SCORES_INJECTION_CONTACT_CATEGORIES=(AA)
LOCAL_SCORES_INJECTION_WINDOW="3"
COMBINED_INTER_RESIDUE_CONTACT_PLOTS_CATEGORIES=(AA)

###########################################

TARGET_FILE=$1
TARGET_NAME=$(basename $TARGET_FILE)
MODEL_FILE=$2
MODEL_NAME=$(basename $MODEL_FILE)
DOMAIN_DIRECTORY=$3"/domain_"$INCLUDE_HETEROATOMS"_"$SUBDIVISION_DEPTH"_"$PROBE_RADIUS
PARENT_OUTPUT_DIRECTORY=$DOMAIN_DIRECTORY"/"$TARGET_NAME
OUTPUT_DIRECTORY=$PARENT_OUTPUT_DIRECTORY"/"$MODEL_NAME

###########################################

mkdir -p $OUTPUT_DIRECTORY

TARGET_ALL_ATOMS_FILE=$PARENT_OUTPUT_DIRECTORY/all_atoms
test -f $TARGET_ALL_ATOMS_FILE || cat $TARGET_FILE | ./voroprot2 --mode collect-atoms --radius-classes ./resources/vdwr_classes.txt --radius-members ./resources/vdwr_members.txt --include-heteroatoms $INCLUDE_HETEROATOMS --include-water 0 > $TARGET_ALL_ATOMS_FILE

TARGET_RESIDUE_IDS_FILE=$PARENT_OUTPUT_DIRECTORY/residue_ids
test -f $TARGET_RESIDUE_IDS_FILE || cat $TARGET_ALL_ATOMS_FILE | ./voroprot2 --mode collect-residue-ids  > $TARGET_RESIDUE_IDS_FILE

TARGET_INTER_ATOM_CONTACTS_FILE=$PARENT_OUTPUT_DIRECTORY/inter_atom_contacts
test -f $TARGET_INTER_ATOM_CONTACTS_FILE || cat $TARGET_ALL_ATOMS_FILE | timeout $CALCULATION_TIMEOUT ./voroprot2 --mode construct-inter-atom-contacts --depth $SUBDIVISION_DEPTH --probe $PROBE_RADIUS > $TARGET_INTER_ATOM_CONTACTS_FILE

MODEL_ALL_ATOMS_FILE=$OUTPUT_DIRECTORY/all_atoms
test -f $MODEL_ALL_ATOMS_FILE || cat $MODEL_FILE | ./voroprot2 --mode collect-atoms --radius-classes ./resources/vdwr_classes.txt --radius-members ./resources/vdwr_members.txt --include-heteroatoms $INCLUDE_HETEROATOMS --include-water 0 > $MODEL_ALL_ATOMS_FILE

MODEL_SELECTED_ATOMS_FILE=$OUTPUT_DIRECTORY/selected_atoms
test -f $MODEL_SELECTED_ATOMS_FILE || cat $TARGET_ALL_ATOMS_FILE $MODEL_ALL_ATOMS_FILE | ./voroprot2 --mode filter-atoms-by-target > $MODEL_SELECTED_ATOMS_FILE

MODEL_RESIDUE_IDS_FILE=$OUTPUT_DIRECTORY/selected_residue_ids
test -f $MODEL_RESIDUE_IDS_FILE || cat $MODEL_SELECTED_ATOMS_FILE | ./voroprot2 --mode collect-residue-ids  > $MODEL_RESIDUE_IDS_FILE

MODEL_INTER_ATOM_CONTACTS_FILE=$OUTPUT_DIRECTORY/inter_atom_contacts_of_model_$MODEL_NAME
test -f $MODEL_INTER_ATOM_CONTACTS_FILE || cat $MODEL_SELECTED_ATOMS_FILE | timeout $CALCULATION_TIMEOUT ./voroprot2 --mode construct-inter-atom-contacts --depth $SUBDIVISION_DEPTH --probe $PROBE_RADIUS > $MODEL_INTER_ATOM_CONTACTS_FILE

for SCORING_MODE in ${SCORING_MODES[*]}
do
  CAD_PROFILE_FILE=$OUTPUT_DIRECTORY/cad_profile_$SCORING_MODE
  test -f $CAD_PROFILE_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | ./voroprot2 --mode calculate-contact-area-difference-profile --intersect-sequences 0 --scoring-mode $SCORING_MODE > $CAD_PROFILE_FILE

  GLOBAL_SCORES_FILE=$OUTPUT_DIRECTORY/global_scores_$SCORING_MODE
  test -f $GLOBAL_SCORES_FILE || cat $CAD_PROFILE_FILE | ./voroprot2 --mode calculate-contact-area-difference-global-score --use-min 1 --prefix sm$SCORING_MODE > $GLOBAL_SCORES_FILE

  for CONTACT_CATEGORY in ${LOCAL_SCORES_PLOTS_CONTACT_CATEGORIES[*]}
  do
  	LOCAL_SCORES_PLOT_FILE=$OUTPUT_DIRECTORY/local_scores_plot/$SCORING_MODE"_"$CONTACT_CATEGORY"_"$LOCAL_SCORES_PLOTS_MAX_WINDOW.ppm
    if [ ! -f $LOCAL_SCORES_PLOT_FILE.png ];
    then
  	  mkdir -p $OUTPUT_DIRECTORY/local_scores_plot/
      cat $CAD_PROFILE_FILE | ./voroprot2 --mode print-contact-area-difference-local-scores-plot --category $CONTACT_CATEGORY --max-window $LOCAL_SCORES_PLOTS_MAX_WINDOW > $LOCAL_SCORES_PLOT_FILE
      convert $LOCAL_SCORES_PLOT_FILE $LOCAL_SCORES_PLOT_FILE.png
      rm $LOCAL_SCORES_PLOT_FILE
    fi
  done
  
  for CONTACT_CATEGORY in ${LOCAL_SCORES_INJECTION_CONTACT_CATEGORIES[*]}
  do
  	LOCAL_SCORES_INJECTED_TO_MODEL_PDB_FILE=$OUTPUT_DIRECTORY/local_scores_injected_to_model/$SCORING_MODE"_"$CONTACT_CATEGORY"_"$LOCAL_SCORES_INJECTION_WINDOW.pdb
    if [ ! -f $LOCAL_SCORES_INJECTED_TO_MODEL_PDB_FILE ];
    then
      mkdir -p $OUTPUT_DIRECTORY/local_scores_injected_to_model/
      (cat $CAD_PROFILE_FILE | ./voroprot2 --mode calculate-contact-area-difference-local-scores --category $CONTACT_CATEGORY --window $LOCAL_SCORES_INJECTION_WINDOW ; cat $MODEL_FILE) | ./voroprot2 --mode print-contact-area-difference-local-scores-injected-to-pdb-file > $LOCAL_SCORES_INJECTED_TO_MODEL_PDB_FILE
    fi
  done
done

COMBINED_INTER_RESIDUE_CONTACTS_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts
test -f $COMBINED_INTER_RESIDUE_CONTACTS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | ./voroprot2 --mode combine-inter-residue-contacts > $COMBINED_INTER_RESIDUE_CONTACTS_FILE

for CONTACT_CATEGORY in ${COMBINED_INTER_RESIDUE_CONTACT_PLOTS_CATEGORIES[*]}
do
  COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts_plot/$CONTACT_CATEGORY".ppm"
  if [ ! -f $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE.png ];
  then
    mkdir -p $OUTPUT_DIRECTORY/combined_inter_residue_contacts_plot/
    cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE | ./voroprot2 --mode print-combined-inter-residue-contacts-plot --category $CONTACT_CATEGORY > $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE
    convert $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE".png"
    rm $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE
  fi
done

DESCRIPTION_FILE=$OUTPUT_DIRECTORY/description
echo "target $TARGET_NAME" > $DESCRIPTION_FILE
echo "model $MODEL_NAME" >> $DESCRIPTION_FILE
echo target_atoms_count `sed -n '2p' $TARGET_ALL_ATOMS_FILE` >> $DESCRIPTION_FILE
echo model_atoms_count `sed -n '2p' $MODEL_SELECTED_ATOMS_FILE` >> $DESCRIPTION_FILE
echo target_residues_count `sed -n '2p' $TARGET_RESIDUE_IDS_FILE` >> $DESCRIPTION_FILE
echo model_residues_count `sed -n '2p' $MODEL_RESIDUE_IDS_FILE` >> $DESCRIPTION_FILE

TMSCORE_PROFILE_FILE=$OUTPUT_DIRECTORY/tm_score_profile
TMscore $MODEL_FILE $TARGET_FILE > $TMSCORE_PROFILE_FILE
if grep -q "TM-score" $TMSCORE_PROFILE_FILE
then
  TMSCORE_RESULTS_FILE=$OUTPUT_DIRECTORY/tm_score_results
  echo TM_score `cat $TMSCORE_PROFILE_FILE | egrep "TM-score\s*=.*d0" | sed 's/TM-score\s*=\s*\(.*\)\s*(.*/\1/g'` > $TMSCORE_RESULTS_FILE
  echo TM_score_GDT_TS `cat $TMSCORE_PROFILE_FILE | egrep "GDT-TS-score" | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g'` >> $TMSCORE_RESULTS_FILE
  echo TM_score_GDT_HA `cat $TMSCORE_PROFILE_FILE | egrep "GDT-HA-score" | sed 's/GDT-HA-score\s*=\s*\(.*\)\s*%(d<0\.5).*/\1/g'` >> $TMSCORE_RESULTS_FILE
fi

SUMMARY_FILE=$OUTPUT_DIRECTORY/summary_list
cat $DESCRIPTION_FILE $OUTPUT_DIRECTORY/global_scores_* $OUTPUT_DIRECTORY/tm_score_results > $SUMMARY_FILE

TABLE_SUMMARY_FILE=$OUTPUT_DIRECTORY/summary_table_row
for i in 1 2 ; do cat $SUMMARY_FILE | cut --delimiter " " --fields $i | paste -s ; done > $TABLE_SUMMARY_FILE
