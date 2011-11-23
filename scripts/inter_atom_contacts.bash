#!/bin/bash

INCLUDE_HETEROATOMS="0"
SUBDIVISION_DEPTH="3"
PROBE_RADIUS="1.4"

TARGET_FILE=$1

cat $TARGET_FILE | \
./voroprot2 --mode collect-atoms --include-heteroatoms $INCLUDE_HETEROATOMS --include-water 0 | \
./voroprot2 --mode construct-inter-atom-contacts --depth $SUBDIVISION_DEPTH --probe $PROBE_RADIUS
