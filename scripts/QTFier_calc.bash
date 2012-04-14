#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -f    path to input file in PBD format
  -l    flag to include heteroatoms (optional)
  
Note: $0 needs QTFier application
      (http://voronoi.hanyang.ac.kr/software.htm#QTfier)
      to be available either in the same directory as this script
      or in your system binary path

EOF
}

SCRIPT_DIRECTORY=$(dirname $0)
QTFIER_BIN_NAME="QTFier"
QTFIER_BIN="$SCRIPT_DIRECTORY/$QTFIER_BIN_NAME"
if [ ! -f "$QTFIER_BIN" ]
then
  if which $QTFIER_BIN_NAME &> /dev/null
  then
    QTFIER_BIN=$QTFIER_BIN_NAME
  else
    echo "Fatal error: '$QTFIER_BIN_NAME' executable not found" 1>&2
    exit 1
  fi
fi

INPUT_FILE=""
LINES_PATTERN='^ATOM'

while getopts "hf:l" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    f)
      INPUT_FILE=$OPTARG
      ;;
    l)
      LINES_PATTERN='^ATOM|^HETATM'
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$INPUT_FILE" ]
then
  print_help
  exit 1
fi

TEMP_FILE=$(mktemp)
rm $TEMP_FILE
TEMP_INPUT_FILE="$TEMP_FILE.pdb"
TEMP_INPUT_FILE_BASE=$(basename $TEMP_INPUT_FILE .pdb)
cat $INPUT_FILE | egrep $LINES_PATTERN > $TEMP_INPUT_FILE

TEMP_OUTPUT_DIR="$(mktemp -d)/"
TEMP_OUTPUT_FILE="$TEMP_OUTPUT_DIR$TEMP_INPUT_FILE_BASE.a.qtf"

$QTFIER_BIN $TEMP_INPUT_FILE $TEMP_OUTPUT_DIR 1>&2
cat $TEMP_OUTPUT_FILE
rm $TEMP_INPUT_FILE
rm -r $TEMP_OUTPUT_DIR
