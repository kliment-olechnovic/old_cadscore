#!/bin/bash

TARGET_FILE=$1
TARGET_NAME=$(basename $TARGET_FILE)
MODEL_FILE=$2
MODEL_NAME=$(basename $MODEL_FILE)

DOMAIN_DIRECTORY=$3"/tm_score"

###########################################

OUTPUT_DIRECTORY=$DOMAIN_DIRECTORY"/"$TARGET_NAME"/"$MODEL_NAME
mkdir -p $OUTPUT_DIRECTORY

TMSCORE_PROFILE_FILE=$OUTPUT_DIRECTORY/profile
test -f $TMSCORE_PROFILE_FILE || TMscore $MODEL_FILE $TARGET_FILE > $TMSCORE_PROFILE_FILE 2> $TMSCORE_PROFILE_FILE.log
if grep -q "TM-score" $TMSCORE_PROFILE_FILE
then  
  TMSCORE_SCORE_SUMMARY_LIST_FILE=$OUTPUT_DIRECTORY/summary_list
  echo "target $TARGET_NAME" > $TMSCORE_SCORE_SUMMARY_LIST_FILE
  echo "model $MODEL_NAME" >> $TMSCORE_SCORE_SUMMARY_LIST_FILE
  echo TM_score `cat $TMSCORE_PROFILE_FILE | egrep "TM-score\s*=.*d0" | sed 's/TM-score\s*=\s*\(.*\)\s*(.*/\1/g'` >> $TMSCORE_SCORE_SUMMARY_LIST_FILE
  echo TM_score_GDT_TS `cat $TMSCORE_PROFILE_FILE | egrep "GDT-TS-score" | sed 's/GDT-TS-score\s*=\s*\(.*\)\s*%(d<1).*/\1/g'` >> $TMSCORE_SCORE_SUMMARY_LIST_FILE
  echo TM_score_GDT_HA `cat $TMSCORE_PROFILE_FILE | egrep "GDT-HA-score" | sed 's/GDT-HA-score\s*=\s*\(.*\)\s*%(d<0\.5).*/\1/g'` >> $TMSCORE_SCORE_SUMMARY_LIST_FILE

  TMSCORE_SUMMARY_TABLE_FILE=$OUTPUT_DIRECTORY/summary_table
  for i in 1 2 ; do cat $TMSCORE_SCORE_SUMMARY_LIST_FILE | cut --delimiter " " --fields $i | paste -s ; done > $TMSCORE_SUMMARY_TABLE_FILE
fi
