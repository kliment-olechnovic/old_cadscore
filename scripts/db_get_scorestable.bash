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

LIST_FILE="$TMP_DIR/list"

NEED_TO_PRINT_HEADER=true

for P in `find $DATABASE/cadscore/ -name "global_scores" | sed "s,$DATABASE/cadscore/\(.*\),\1,"`
do
  TARGET_NAME=$(echo $P | cut --delimiter '/' --fields 1);
  MODEL_NAME=$(echo $P | cut --delimiter '/' --fields 2);
  
  echo "Log for target $TARGET_NAME model $MODEL_NAME:" 1>&2
  for L in `find $DATABASE -path "*$TARGET_NAME*$MODEL_NAME*.log" -type f ! -size 0`
  do
    echo "$L" | sed "s,$DATABASE,," | sed 's/^/\t/' 1>&2
    cat $L | sed 's/^/\t\t/' 1>&2
  done
  echo 1>&2
  
  TARGET_ATOMS_FILE="$DATABASE/atoms/$TARGET_NAME/atoms"
  [ -s "$TARGET_ATOMS_FILE" ] || continue
  
  MODEL_ATOMS_FILE="$DATABASE/atoms/$MODEL_NAME/$TARGET_NAME/atoms"
  [ -s "$MODEL_ATOMS_FILE" ] || continue
  
  TARGET_RESIDUE_IDS_FILE="$DATABASE/atoms/$TARGET_NAME/residue_ids"
  [ -s "$TARGET_RESIDUE_IDS_FILE" ] || continue
  
  MODEL_RESIDUE_IDS_FILE="$DATABASE/atoms/$MODEL_NAME/$TARGET_NAME/residue_ids"
  [ -s "$MODEL_RESIDUE_IDS_FILE" ] || continue
  
  CADSCORE_FILE="$DATABASE/cadscore/$TARGET_NAME/$MODEL_NAME/$TARGET_NAME/global_scores"
  [ -s "$CADSCORE_FILE" ] || continue
  
  TMSCORE_FILE="$DATABASE/tmscore/$TARGET_NAME/$MODEL_NAME/tmscore_summary"
  [ -s "$TMSCORE_FILE" ] || continue
  
  LGASCORE_FILE="$DATABASE/lgascore/$TARGET_NAME/$MODEL_NAME/lgascore_summary"
  [ -s "$LGASCORE_FILE" ] || continue
  
  MOLPROBITY_FILE="$DATABASE/molprobity/$TARGET_NAME/$MODEL_NAME/molprobity_summary"
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
