#!/bin/bash

MCANNOTATE="./MC-Annotate"

INPUT=$1

$MCANNOTATE $INPUT | sed -n -e '/Base-pairs/,/-------/ p' | egrep " : " | sed "s/^/$(basename $INPUT) /"
