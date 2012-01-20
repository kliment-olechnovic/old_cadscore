#!/bin/bash

DATADIR="./data/"

APPFILE=$1

for f in `find $DATADIR -name "*.pdb"`
do
  $APPFILE $f
done
