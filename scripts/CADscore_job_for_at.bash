#!/bin/bash

CADSCORE_JOB_DIR=$1
CADSCORE_SCRIPTS_DIR=$2
CADSCORE_CALC_OPTIONS=$3

TMP_FILE=$(mktemp)

cat << EOF > $TMP_FILE
export CADSCORE_JOB_DIR=$CADSCORE_JOB_DIR
export CADSCORE_SCRIPTS_DIR=$CADSCORE_SCRIPTS_DIR
export CADSCORE_CALC_OPTIONS=$CADSCORE_CALC_OPTIONS
$CADSCORE_SCRIPTS_DIR/CADscore_job_for_PBS.bash
EOF

at -f $TMP_FILE now

rm $TMP_FILE