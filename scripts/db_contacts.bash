#!/bin/bash

SCRIPT_DIRECTORY=$(dirname $0)

VOROPROT=$SCRIPT_DIRECTORY/voroprot2

SUBDIVISION_DEPTH="3"
PROBE_RADIUS="1.4"
CALCULATION_TIMEOUT="300s"

###########################################

print_help()
{
cat << EOF 1>&2

$0 options:

  -h    show this message and exit
  -D    path to writable database directory
  -n    name in the database

EOF
}

DATABASE=""
NAME=""

while getopts “hD:n:” OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    D)
      DATABASE=$OPTARG
      ;;
    n)
      NAME=$OPTARG
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$DATABASE" ] || [ -z "$NAME" ]
then
  print_help
  exit 1
fi

ATOMS_FILE="$DATABASE/atoms/$NAME/atoms"
if [ ! -f "$ATOMS_FILE" ]
then
  echo "Atoms file for \"$NAME\" is not in the database" 1>&2
  exit 1
fi

###########################################

OUTPUT_DIRECTORY="$DATABASE/contacts/$NAME"
mkdir -p $OUTPUT_DIRECTORY

CONTACTS_FILE="$OUTPUT_DIRECTORY/inter_atom_contacts"
test -f $CONTACTS_FILE || cat $ATOMS_FILE | timeout $CALCULATION_TIMEOUT $VOROPROT --mode construct-inter-atom-contacts --depth $SUBDIVISION_DEPTH --probe $PROBE_RADIUS > $CONTACTS_FILE 2> "$CONTACTS_FILE.log"
