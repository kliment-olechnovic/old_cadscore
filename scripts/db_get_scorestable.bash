#!/bin/bash

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to existing database directory
  
EOF
}

DATABASE=""

while getopts "hD:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ]
then
  print_help
  exit 1
fi

if [ ! -d "$DATABASE" ]
then
  echo "Database directory \"$DATABASE\" does not exist" 1>&2
  exit 1
fi

###########################################

TMP_DIR=$(mktemp -d)

LOG_POOL_FILE="$TMP_DIR/log_pool"
LIST_FILE="$TMP_DIR/list"

NEED_TO_PRINT_HEADER=true

for P in `find $DATABASE/cadscore/ -name "global_scores" | sed "s,$DATABASE/cadscore/\(.*\),\1,"`
do
  TARGET_NAME=$(echo $P | cut --delimiter '/' --fields 1);
  MODEL_NAME=$(echo $P | cut --delimiter '/' --fields 2);
  
  TARGET_ATOMS_DIR="$DATABASE/atoms/$TARGET_NAME/"
  MODEL_ATOMS_DIR="$DATABASE/atoms/$MODEL_NAME/$TARGET_NAME/"
  TARGET_CONTACTS_DIR="$DATABASE/contacts/$TARGET_NAME/"
  MODEL_CONTACTS_DIR="$DATABASE/contacts/$MODEL_NAME/$TARGET_NAME/"
  CADSCORE_DIR="$DATABASE/cadscore/$TARGET_NAME/$MODEL_NAME/$TARGET_NAME/"
  TMSCORE_DIR="$DATABASE/tmscore/$TARGET_NAME/$MODEL_NAME/"
  LGASCORE_DIR="$DATABASE/lgascore/$TARGET_NAME/$MODEL_NAME/"
  MOLPROBITY_DIR="$DATABASE/molprobity/$TARGET_NAME/$MODEL_NAME/"
  
  true > $LOG_POOL_FILE
  for L in `find $TARGET_ATOMS_DIR $MODEL_ATOMS_DIR $TARGET_CONTACTS_DIR $MODEL_CONTACTS_DIR $CADSCORE_DIR $TMSCORE_DIR $LGASCORE_DIR -name "*.log" -type f ! -size 0`
  do
    basename "$L" >> $LOG_POOL_FILE
    cat $L | sed 's/^/\t/' >> $LOG_POOL_FILE
  done
  
  if [ -s "$LOG_POOL_FILE" ]
  then
    echo "Log for target $TARGET_NAME model $MODEL_NAME" 1>&2
    cat $LOG_POOL_FILE | sed 's/^/\t/' 1>&2
    echo 1>&2
  fi
  
  TARGET_ATOMS_FILE="$TARGET_ATOMS_DIR/atoms"
  [ -s "$TARGET_ATOMS_FILE" ] || continue
  
  TARGET_RESIDUE_IDS_FILE="$TARGET_ATOMS_DIR/residue_ids"
  [ -s "$TARGET_RESIDUE_IDS_FILE" ] || continue
  
  MODEL_ATOMS_FILE="$MODEL_ATOMS_DIR/atoms"
  [ -s "$MODEL_ATOMS_FILE" ] || continue
  
  MODEL_RESIDUE_IDS_FILE="$MODEL_ATOMS_DIR/residue_ids"
  [ -s "$MODEL_RESIDUE_IDS_FILE" ] || continue
  
  CADSCORE_FILE="$CADSCORE_DIR/global_scores"
  [ -s "$CADSCORE_FILE" ] || continue
  
  TMSCORE_FILE="$TMSCORE_DIR/tmscore_summary"
  [ -s "$TMSCORE_FILE" ] || continue
  
  LGASCORE_FILE="$LGASCORE_DIR/lgascore_summary"
  [ -s "$LGASCORE_FILE" ] || continue
  
  MOLPROBITY_FILE="$MOLPROBITY_DIR/molprobity_summary"
  [ -s "$MOLPROBITY_FILE" ] || continue
  
  echo target $TARGET_NAME > $LIST_FILE
  echo model $MODEL_NAME >> $LIST_FILE
  echo target_atoms_count `sed -n '2p' $TARGET_ATOMS_FILE` >> $LIST_FILE
  echo model_atoms_count `sed -n '2p' $MODEL_ATOMS_FILE` >> $LIST_FILE
  echo target_residues_count `sed -n '2p' $TARGET_RESIDUE_IDS_FILE` >> $LIST_FILE
  echo model_residues_count `sed -n '2p' $MODEL_RESIDUE_IDS_FILE` >> $LIST_FILE
  cat $CADSCORE_FILE >> $LIST_FILE
  cat $TMSCORE_FILE >> $LIST_FILE
  cat $LGASCORE_FILE >> $LIST_FILE
  cat $MOLPROBITY_FILE >> $LIST_FILE
  
  if $NEED_TO_PRINT_HEADER
  then
    cat $LIST_FILE | cut --delimiter " " --fields 1 | paste -s
    NEED_TO_PRINT_HEADER=false
  fi
  
  cat $LIST_FILE | cut --delimiter " " --fields 2 | paste -s
  
done

rm -r $TMP_DIR
