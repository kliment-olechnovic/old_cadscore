#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -f    path to input file in PBD format

EOF
}

SCRIPT_DIRECTORY=$(dirname $0)

VOROPROT_CALC_SCRIPT="$SCRIPT_DIRECTORY/Voroprot2_calc.bash"
if [ ! -f "$VOROPROT_CALC_SCRIPT" ]
then
  echo "Fatal error: '$VOROPROT_CALC_SCRIPT' executable not found" 1>&2
  exit 1
fi

QTFIER_CALC_SCRIPT="$SCRIPT_DIRECTORY/QTFier_calc.bash"
if [ ! -f "$QTFIER_CALC_SCRIPT" ]
then
  echo "Fatal error: '$QTFIER_CALC_SCRIPT' executable not found" 1>&2
  exit 1
fi


MODEL_FILE=""

while getopts "hf:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    f)
      MODEL_FILE=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$MODEL_FILE" ]
then
  print_help
  exit 1
fi

if [ ! -f "$MODEL_FILE" ]
then
  echo "File \"$MODEL_FILE\" does not exist" 1>&2
  exit 1
fi

echo "input $(basename $MODEL_FILE)"

TEMP_FILE_NAME=$(mktemp)
rm $TEMP_FILE_NAME
DATA_FILE_NAME="$TEMP_FILE_NAME.data"
TIME_FILE_NAME="$TEMP_FILE_NAME.time"
LOG_FILE_NAME="$TEMP_FILE_NAME.log"

( time -p ( ( $VOROPROT_CALC_SCRIPT -f "$MODEL_FILE" -q -v $SCRIPT_DIRECTORY/../resources/simplified 2> $LOG_FILE_NAME | head -3 ) > $DATA_FILE_NAME ) ) 2> $TIME_FILE_NAME
cat $DATA_FILE_NAME | egrep '^Atoms|^Tangent' | sed 's/Atoms count:/voroprot2_atoms/' | sed 's/Tangent spheres count:/voroprot2_vertices/'
cat $TIME_FILE_NAME | egrep '^user' | sed 's/user/voroprot2_time/'
cat $LOG_FILE_NAME | sed 's/^/#voroprot2_log /'

( time -p ( ( $QTFIER_CALC_SCRIPT -f "$MODEL_FILE" 2> $LOG_FILE_NAME | head -11 ) > $DATA_FILE_NAME ) ) 2> $TIME_FILE_NAME

cat $DATA_FILE_NAME | egrep '^MODEL' | awk '{print "qtfier_atoms " $3}'
cat $DATA_FILE_NAME | egrep '^MODEL' | awk '{print "qtfier_vertices " $4}'
cat $TIME_FILE_NAME | egrep '^user' | sed 's/user/qtfier_time/'
cat $LOG_FILE_NAME | egrep -v '^input|^output|model is generated' | sed 's/^/#qtfier_log /'

rm $DATA_FILE_NAME
rm $TIME_FILE_NAME
rm $LOG_FILE_NAME
