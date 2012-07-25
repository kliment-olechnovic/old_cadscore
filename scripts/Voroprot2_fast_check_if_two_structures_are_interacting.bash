#!/bin/bash

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -a    path to first input file in PBD format
  -b    path to second input file in PBD format

EOF
}

SCRIPT_DIRECTORY=$(dirname $0)
VOROPROT_NAME="voroprot2"
VOROPROT="$SCRIPT_DIRECTORY/$VOROPROT_NAME"
if [ ! -f "$VOROPROT" ]
then
  if which $VOROPROT_NAME &> /dev/null
  then
    VOROPROT=$VOROPROT_NAME
  else
    echo "Fatal error: '$VOROPROT_NAME' executable not found" 1>&2
    exit 1
  fi
fi

MODEL_FILE_A=""
MODEL_FILE_B=""

while getopts "ha:b:" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    a)
      MODEL_FILE_A=$OPTARG
      ;;
    b)
      MODEL_FILE_B=$OPTARG
      ;;
  esac
done

if [ -z "$MODEL_FILE_A" ] || [ -z "$MODEL_FILE_B" ]
then
  print_help
  exit 1
fi

if [ ! -f "$MODEL_FILE_A" ]
then
  echo "File \"$MODEL_FILE_A\" does not exist" 1>&2
  exit 1
fi

if [ ! -f "$MODEL_FILE_B" ]
then
  echo "File \"$MODEL_FILE_B\" does not exist" 1>&2
  exit 1
fi

echo "$MODEL_FILE_A $MODEL_FILE_B" | $VOROPROT --mode check-for-any-inter-chain-contact --direct-reading
