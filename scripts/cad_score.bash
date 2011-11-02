#!/bin/bash

TARGET_FILE=$1
TARGET_NAME=$(basename $TARGET_FILE)

MODEL_FILE=$2
MODEL_NAME=$(basename $MODEL_FILE)

OUTPUT_DIRECTORY=$3

BOTH_NAMES=$TARGET_NAME"_and_"$MODEL_NAME

TARGET_ALL_ATOMS_FILE=$OUTPUT_DIRECTORY/all_atoms_of_target_$TARGET_NAME
if [ ! -f $TARGET_ALL_ATOMS_FILE ];
then
  cat $TARGET_FILE | ./voroprot2 --mode collect-atoms --radius-classes ./resources/vdwr_classes.txt --radius-members ./resources/vdwr_members.txt --include-heteroatoms 0 --include-water 0 > $TARGET_ALL_ATOMS_FILE
fi

TARGET_INTER_ATOM_CONTACTS_FILE=$OUTPUT_DIRECTORY/inter_atom_contacts_of_target_$TARGET_NAME
if [ ! -f $TARGET_INTER_ATOM_CONTACTS_FILE ];
then
  cat $TARGET_ALL_ATOMS_FILE | ./voroprot2 --mode construct-inter-atom-contacts --depth 3 --probe 1.4 > $TARGET_INTER_ATOM_CONTACTS_FILE
fi

MODEL_ALL_ATOMS_FILE=$OUTPUT_DIRECTORY/all_atoms_of_model_$MODEL_NAME
cat $MODEL_FILE | ./voroprot2 --mode collect-atoms --radius-classes ./resources/vdwr_classes.txt --radius-members ./resources/vdwr_members.txt --include-heteroatoms 0 --include-water 0 > $MODEL_ALL_ATOMS_FILE

MODEL_SELECTED_ATOMS_FILE=$OUTPUT_DIRECTORY/selected_atoms_of_model_$MODEL_NAME
cat $TARGET_ALL_ATOMS_FILE $MODEL_ALL_ATOMS_FILE | ./voroprot2 --mode filter-atoms-by-target > $MODEL_SELECTED_ATOMS_FILE
#rm $MODEL_ALL_ATOMS_FILE

MODEL_INTER_ATOM_CONTACTS_FILE=$OUTPUT_DIRECTORY/inter_atom_contacts_of_model_$MODEL_NAME
cat $MODEL_SELECTED_ATOMS_FILE | ./voroprot2 --mode construct-inter-atom-contacts --depth 3 --probe 1.4 > $MODEL_INTER_ATOM_CONTACTS_FILE

CAD_PROFILE_FILE=$OUTPUT_DIRECTORY/cad_profile_of_$BOTH_NAMES
cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | ./voroprot2 --mode calculate-contact-area-difference-profile --intersect-sequences 0 --scoring-mode 0 > $CAD_PROFILE_FILE

COMBINED_INTER_RESIDUE_CONTACTS_FILE=$OUTPUT_DIRECTORY/combined_inter_residue_contacts_of_$BOTH_NAMES
cat $TARGET_INTER_ATOM_CONTACTS_FILE $MODEL_INTER_ATOM_CONTACTS_FILE | ./voroprot2 --mode combine-inter-residue-contacts > $COMBINED_INTER_RESIDUE_CONTACTS_FILE

GLOBAL_SCORES_FILE=$OUTPUT_DIRECTORY/global_scores_of_$BOTH_NAMES
(echo "target $TARGET_NAME"; echo "model $MODEL_NAME"; cat $CAD_PROFILE_FILE | ./voroprot2 --mode calculate-contact-area-difference-global-score --use-min 0) > $GLOBAL_SCORES_FILE

CONTACT_CATEGORIES=(AA AS SA SS)
for CONTACT_CATEGORY in ${CONTACT_CATEGORIES[*]}
do
  LOCAL_SCORES_PLOT_FILE=$OUTPUT_DIRECTORY/local_scores_plot_$CONTACT_CATEGORY"_"of_$BOTH_NAMES.ppm
  cat $CAD_PROFILE_FILE | ./voroprot2 --mode print-contact-area-difference-local-scores-plot --category $CONTACT_CATEGORY --max-window 30 > $LOCAL_SCORES_PLOT_FILE
  COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PPM=$OUTPUT_DIRECTORY/combined_inter_residue_contacts_plot_$CONTACT_CATEGORY"_"of_$BOTH_NAMES.ppm
  cat $COMBINED_INTER_RESIDUE_CONTACTS_FILE | ./voroprot2 --mode print-combined-inter-residue-contacts-plot --category $CONTACT_CATEGORY > $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PPM
#  COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PNG=$COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PPM".png"
#  convert $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PPM $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PNG
#  rm $COMBINED_INTER_RESIDUE_CONTACTS_PLOT_FILE_PPM
done
