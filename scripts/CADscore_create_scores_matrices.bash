#!/bin/bash

##################################################
### Help printing

print_help()
{
cat << EOF 1>&2

$0 parameters:

  Required:
    -I    path to input directory
    -O    path to writable output directory

  Optional:
    -l    flag to include heteroatoms
    -c    flag to consider only inter-chain contacts
    -i    inter-interval contacts specification
    -d    flag to produce dendrograms with R

  Other:
    -h    show this message and exit

EOF
}

##################################################
### Reading and checking arguments

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

INPUT_DIR=""
DATABASE=""
HETATM_FLAG=""
INTER_CHAIN_FLAG=""
INTER_INTERVAL_OPTION=""
CREATE_DENDROGRAMS=false

while getopts "hI:O:lci:d" OPTION
do
  case $OPTION in
    h)
      print_help
      exit 0
      ;;
    I)
      INPUT_DIR=$OPTARG
      ;;
    O)
      DATABASE=$OPTARG
      ;;
    l)
      HETATM_FLAG="--HETATM"
      ;;
    c)
      INTER_CHAIN_FLAG="--inter-chain"
      ;;
    i)
      INTER_INTERVAL_OPTION="--inter-interval "$OPTARG
      ;;
    d)
      CREATE_DENDROGRAMS=true
      ;;
    ?)
      exit 1
      ;;
  esac
done

if [ -z "$INPUT_DIR" ] || [ -z "$DATABASE" ]
then
  print_help
  exit 1
fi

if [ ! -d "$INPUT_DIR" ]
then
  echo "Fatal error: input directory \"$INPUT_DIR\" does not exist" 1>&2
  exit 1
fi

##################################################
### Calculating inter-atom contacts

find "$INPUT_DIR" -mindepth 1 -maxdepth 1 -type f | sort | while read TARGET_FILE
do
  TARGET_NAME=$(basename $TARGET_FILE)
  TARGET_DIR="$DATABASE/$TARGET_NAME"
  TARGET_INTER_ATOM_CONTACTS_FILE="$TARGET_DIR/inter_atom_contacts"
  TARGET_RESIDUE_IDS_FILE="$TARGET_DIR/residue_ids"
  
  mkdir -p "$TARGET_DIR"
  
  test -f $TARGET_INTER_ATOM_CONTACTS_FILE || cat $TARGET_FILE | $VOROPROT --mode collect-atoms $HETATM_FLAG | $VOROPROT --mode calc-inter-atom-contacts > $TARGET_INTER_ATOM_CONTACTS_FILE
  if [ ! -s "$TARGET_INTER_ATOM_CONTACTS_FILE" ] ; then echo "Fatal error: no inter-atom contacts in the target" 1>&2 ; exit 1 ; fi

  test -f $TARGET_RESIDUE_IDS_FILE || cat $TARGET_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode collect-residue-ids  > $TARGET_RESIDUE_IDS_FILE
  if [ ! -s "$TARGET_RESIDUE_IDS_FILE" ] ; then echo "Fatal error: no residues in the target" 1>&2 ; exit 1 ; fi
done

##################################################
### Calculating scores

SCORES_LIST_FILE="$DATABASE/scores_list"

if [ ! -f "$SCORES_LIST_FILE" ]
then
  find "$INPUT_DIR" -mindepth 1 -maxdepth 1 -type f | sort | while read TARGET_FILE
  do
    TARGET_NAME=$(basename $TARGET_FILE)
    TARGET_DIR="$DATABASE/$TARGET_NAME"
    TARGET_INTER_ATOM_CONTACTS_FILE="$TARGET_DIR/inter_atom_contacts"
    TARGET_RESIDUE_IDS_FILE="$TARGET_DIR/residue_ids"
  
    find "$INPUT_DIR" -mindepth 1 -maxdepth 1 -type f | sort | while read MODEL_FILE
    do
  	  MODEL_NAME=$(basename $MODEL_FILE)
  	  MODEL_INTER_ATOM_CONTACTS_FILE="$DATABASE/$MODEL_NAME/inter_atom_contacts"
  	
  	  echo TARGET_ROLE $TARGET_NAME >> $SCORES_LIST_FILE
      echo MODEL_ROLE $MODEL_NAME >> $SCORES_LIST_FILE
  	  ( (cat $TARGET_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-inter-residue-contacts ; cat $MODEL_INTER_ATOM_CONTACTS_FILE | $VOROPROT --mode calc-inter-residue-contacts) | $VOROPROT --mode calc-combined-inter-residue-contacts $INTER_CHAIN_FLAG $INTER_INTERVAL_OPTION ; cat $TARGET_RESIDUE_IDS_FILE ) | $VOROPROT --mode calc-CAD-profile | $VOROPROT --mode calc-CAD-global-scores | grep -v "_diff" | grep -v "_ref" >> $SCORES_LIST_FILE
    done
 done
fi

##################################################
### Forming scores matrix

CATEGORIES=("AA" "AS" "SS")

for CATEGORY in "${CATEGORIES[@]}"
do
  SCORES_MATRIX_FILE="$DATABASE/scores_matrix_$CATEGORY"
  if [ ! -f "$SCORES_MATRIX_FILE" ]
  then
    cat $SCORES_LIST_FILE | grep "MODEL_ROLE" | cut --delimiter " " --fields 2 | awk 'BEGIN {RS="";FS="\n";ORS=" "}{ dim=sqrt(NF); for (i=0; i<dim; i++) { print $(i+1); } }' >> $SCORES_MATRIX_FILE
    echo "" >> $SCORES_MATRIX_FILE
    cat $SCORES_LIST_FILE | grep "$CATEGORY" | cut --delimiter " " --fields 2 | awk 'BEGIN {RS="";FS="\n";ORS=" "}{ dim=sqrt(NF); for (i=0; i<= NF; i++) { print $(i+1); if(((i+1)%dim )==0) { printf "\n"; } } }' >> $SCORES_MATRIX_FILE
  fi
  
if $CREATE_DENDROGRAMS
then
HEATMAP_IMAGE_FILE="$SCORES_MATRIX_FILE.png"
R --vanilla << EOF
dt=read.table("$SCORES_MATRIX_FILE", header=TRUE);
dt=1-dt;
dm=as.matrix(dt);
row.names(dm)=colnames(dt);
png("$HEATMAP_IMAGE_FILE", height=15, width=15, units="in", res=200)
#heatmap(dm, col=topo.colors(length(dt[[1]])), margins=c(15,15));
heatmap(dm, margins=c(15,15));
dev.off()
EOF
fi

done
