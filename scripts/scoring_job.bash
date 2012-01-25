#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -J    path to job directory
  -g    flag to run TMScore
  -p    flag to run MolProbity

EOF
}

JOB_DIRECTORY=""
FLAG_TMSCORE=""
FLAG_MOLPROBITY=""

while getopts "hJ:gp" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    J)
      JOB_DIRECTORY=$OPTARG
      ;;
    g)
      FLAG_TMSCORE="-g"
      ;;
    p)
      FLAG_MOLPROBITY="-p"
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$JOB_DIRECTORY" ]
then
  print_help
  exit 1
fi

if [ ! -d "$JOB_DIRECTORY" ]
then
  echo "Job directory \"$JOB_DIRECTORY\" does not exist" 1>&2
  exit 1
fi

###########################################

$SCRIPT_DIRECTORY/db_set_multiple_scoring_reports.bash -D $JOB_DIRECTORY/db -T $JOB_DIRECTORY/target -M $JOB_DIRECTORY/model $FLAG_TMSCORE $FLAG_MOLPROBITY

$SCRIPT_DIRECTORY/db_set_scoring_summary.bash -D $JOB_DIRECTORY/db
