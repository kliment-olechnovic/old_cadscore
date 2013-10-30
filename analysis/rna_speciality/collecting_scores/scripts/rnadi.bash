#!/bin/bash

INTARGET=$1
INMODEL=$2
TEMPDIR=$(mktemp -d)

cp $INTARGET $TEMPDIR/target.pdb
gzip $TEMPDIR/target.pdb

cp $INMODEL $TEMPDIR/model.pdb
gzip $TEMPDIR/model.pdb

cd $TEMPDIR

python ~/bin/rnadi_enhanced_package/inf.py $TEMPDIR $TEMPDIR/target.pdb.gz > $TEMPDIR/inf.txt
python ~/bin/rnadi_enhanced_package/inf_norv.py $TEMPDIR $TEMPDIR/target.pdb.gz > $TEMPDIR/inf_norv.txt
python ~/bin/rnadi_enhanced_package/inf_norv_os.py $TEMPDIR $TEMPDIR/target.pdb.gz > $TEMPDIR/inf_norv_os.txt
python ~/bin/rnadi_enhanced_package/inf_norv_obp.py $TEMPDIR $TEMPDIR/target.pdb.gz > $TEMPDIR/inf_norv_obp.txt
python ~/bin/rnadi_enhanced_package/rmsd.py --decoy_folder $TEMPDIR --reference $TEMPDIR/target.pdb.gz > $TEMPDIR/rmsd.txt

RNAINF=$(cat $TEMPDIR/inf.txt | sed 's/Matthews.*is:\s\(\S*\)\s.*/\1/g')
RNAINF_NORV=$(cat $TEMPDIR/inf_norv.txt | sed 's/Matthews.*is:\s\(\S*\)\s.*/\1/g')
RNAINF_NORV_OS=$(cat $TEMPDIR/inf_norv_os.txt | sed 's/Matthews.*is:\s\(\S*\)\s.*/\1/g')
RNAINF_NORV_OBP=$(cat $TEMPDIR/inf_norv_obp.txt | sed 's/Matthews.*is:\s\(\S*\)\s.*/\1/g')
RNARMSD=$(cat $TEMPDIR/rmsd.txt | sed 's/\s*\S*\s*\(\S*\)\s*Angstrom.*/\1/g')

if [ -z "$RNAINF" ] ; then RNAINF="-1" ; fi
if [ -z "$RNAINF_NORV" ] ; then RNAINF_NORV="-1" ; fi
if [ -z "$RNAINF_NORV_OS" ] ; then RNAINF_NORV_OS="-1" ; fi
if [ -z "$RNAINF_NORV_OBP" ] ; then RNAINF_NORV_OBP="-1" ; fi
if [ -z "$RNARMSD" ] ; then RNARMSD="-1" ; fi

echo "rna_inf $RNAINF"
echo "rna_inf_norv $RNAINF_NORV"
echo "rna_inf_norv_os $RNAINF_NORV_OS"
echo "rna_inf_norv_obp $RNAINF_NORV_OBP"
echo "rna_rmsd $RNARMSD"

rm -r $TEMPDIR
