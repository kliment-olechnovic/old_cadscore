#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -t    path to target file in PBD format
  -m    path to model file in PBD format
  -g    flag to run TMScore
  -l    flag to run LGA
  -p    flag to run MolProbity

EOF
}

DATABASE=""
TARGET_FILE=""
MODEL_FILE=""
FLAG_TMSCORE=false
FLAG_LGA=false
FLAG_MOLPROBITY=false

while getopts "hD:t:m:glp" OPTION
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
    g)
      FLAG_TMSCORE=true
      ;;
    l)
      FLAG_LGA=true
      ;;
    p)
      FLAG_MOLPROBITY=true
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

OUTPUT_DIRECTORY="$DATABASE/scoring_report/$TARGET_NAME/$MODEL_NAME"
mkdir -p $OUTPUT_DIRECTORY

LIST_FILE="$OUTPUT_DIRECTORY/list"
echo target $TARGET_NAME > $LIST_FILE
echo model $MODEL_NAME >> $LIST_FILE

$SCRIPT_DIRECTORY/db_set_atoms.bash -D $DATABASE -i $TARGET_FILE
$SCRIPT_DIRECTORY/db_set_atoms.bash -D $DATABASE -i $MODEL_FILE -t $TARGET_NAME

TARGET_ATOMS_FILE="$DATABASE/atoms/$TARGET_NAME/atoms"
echo target_atoms_count `sed -n '2p' $TARGET_ATOMS_FILE` >> $LIST_FILE

MODEL_ATOMS_FILE="$DATABASE/atoms/$MODEL_NAME/$TARGET_NAME/atoms"
echo model_atoms_count `sed -n '2p' $MODEL_ATOMS_FILE` >> $LIST_FILE

TARGET_RESIDUE_IDS_FILE="$DATABASE/atoms/$TARGET_NAME/residue_ids"
echo target_residues_count `sed -n '2p' $TARGET_RESIDUE_IDS_FILE` >> $LIST_FILE

MODEL_RESIDUE_IDS_FILE="$DATABASE/atoms/$MODEL_NAME/$TARGET_NAME/residue_ids"
echo model_residues_count `sed -n '2p' $MODEL_RESIDUE_IDS_FILE` >> $LIST_FILE

$SCRIPT_DIRECTORY/db_set_contacts.bash -D $DATABASE -n $TARGET_NAME
$SCRIPT_DIRECTORY/db_set_contacts.bash -D $DATABASE -n "$MODEL_NAME/$TARGET_NAME"
$SCRIPT_DIRECTORY/db_set_cadscore.bash -D $DATABASE -t $TARGET_NAME -m "$MODEL_NAME/$TARGET_NAME"

CADSCORE_FILE="$DATABASE/cadscore/$TARGET_NAME/$MODEL_NAME/$TARGET_NAME/global_scores"
cat $CADSCORE_FILE >> $LIST_FILE

if $FLAG_TMSCORE
then
  $SCRIPT_DIRECTORY/db_set_tmscore.bash -D $DATABASE -t $TARGET_FILE -m $MODEL_FILE
  TMSCORE_FILE="$DATABASE/tmscore/$TARGET_NAME/$MODEL_NAME/tmscore_summary"
  cat $TMSCORE_FILE >> $LIST_FILE
fi

if $FLAG_LGA
then
  $SCRIPT_DIRECTORY/db_set_lgascore.bash -D $DATABASE -t $TARGET_FILE -m $MODEL_FILE
  LGASCORE_FILE="$DATABASE/lgascore/$TARGET_NAME/$MODEL_NAME/lgascore_summary"
  cat $LGASCORE_FILE >> $LIST_FILE
fi

if $FLAG_MOLPROBITY
then
  $SCRIPT_DIRECTORY/db_set_molprobity.bash -D $DATABASE -m $MODEL_FILE
  MOLPROBITY_FILE="$DATABASE/molprobity/$MODEL_NAME/molprobity_summary"
  cat $MOLPROBITY_FILE >> $LIST_FILE
fi

TABLE_FILE="$OUTPUT_DIRECTORY/table_row"
$SCRIPT_DIRECTORY/util_convert_list_to_table_row.bash $LIST_FILE > $TABLE_FILE

LOG_POOL_FILE="$OUTPUT_DIRECTORY/log_pool"
true > $LOG_POOL_FILE
for L in `find $DATABASE -path "*$TARGET_NAME*$MODEL_NAME*.log" -type f ! -size 0`
do
  echo "$L" | sed "s,$DATABASE,," >> $LOG_POOL_FILE
  cat $L | sed 's/^/\t/' >> $LOG_POOL_FILE
done
