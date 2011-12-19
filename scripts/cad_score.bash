#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -O    path to writable output directory
  -t    path to target file in PBD format
  -m    path to model file in PBD format

EOF
}

DATABASE=""
TARGET_FILE=""
MODEL_FILE=""

while getopts “hO:t:m:f:” OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    O)
      DATABASE=$OPTARG
      ;;
    t)
      TARGET_FILE=$OPTARG
      ;;
    m)
      MODEL_FILE=$OPTARG
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

###########################################

bash $SCRIPT_DIRECTORY/db_atoms.bash -D $DATABASE -i $TARGET_FILE
bash $SCRIPT_DIRECTORY/db_atoms.bash -D $DATABASE -i $MODEL_FILE -t $TARGET_NAME
bash $SCRIPT_DIRECTORY/db_contacts.bash -D $DATABASE -n $TARGET_NAME
bash $SCRIPT_DIRECTORY/db_contacts.bash -D $DATABASE -n "$MODEL_NAME/$TARGET_NAME"
bash $SCRIPT_DIRECTORY/db_cadscore.bash -D $DATABASE -t $TARGET_NAME -m "$MODEL_NAME/$TARGET_NAME"
bash $SCRIPT_DIRECTORY/db_tmscore.bash -D $DATABASE -t $TARGET_FILE -m $MODEL_FILE

OUTPUT_DIRECTORY="$DATABASE/reports/$TARGET_NAME/$MODEL_NAME"
mkdir -p $OUTPUT_DIRECTORY

LIST_FILE="$OUTPUT_DIRECTORY/cad_score_list"

echo target $TARGET_NAME > $LIST_FILE
echo model $MODEL_NAME >> $LIST_FILE

TARGET_ATOMS_FILE="$DATABASE/atoms/$TARGET_NAME/atoms"
echo target_atoms_count `sed -n '2p' $TARGET_ATOMS_FILE` >> $LIST_FILE

MODEL_ATOMS_FILE="$DATABASE/atoms/$MODEL_NAME/$TARGET_NAME/atoms"
echo model_atoms_count `sed -n '2p' $MODEL_ATOMS_FILE` >> $LIST_FILE

TARGET_RESIDUE_IDS_FILE="$DATABASE/atoms/$TARGET_NAME/residue_ids"
echo target_residues_count `sed -n '2p' $TARGET_RESIDUE_IDS_FILE` >> $LIST_FILE

MODEL_RESIDUE_IDS_FILE="$DATABASE/atoms/$MODEL_NAME/$TARGET_NAME/residue_ids"
echo model_residues_count `sed -n '2p' $MODEL_RESIDUE_IDS_FILE` >> $LIST_FILE

CADSCORE_FILE="$DATABASE/cadscore/$TARGET_NAME/$MODEL_NAME/$TARGET_NAME/global_scores"
cat $CADSCORE_FILE >> $LIST_FILE

TMSCORE_FILE="$DATABASE/tmscore/$TARGET_NAME/$MODEL_NAME/tmscore_summary"
cat $TMSCORE_FILE >> $LIST_FILE

TABLE_FILE="$OUTPUT_DIRECTORY/cad_score_table"
for i in 1 2 ; do cat $LIST_FILE | cut --delimiter " " --fields $i | paste -s ; done > $TABLE_FILE
