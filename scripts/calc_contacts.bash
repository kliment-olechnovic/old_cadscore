#!/bin/bash

./Release/voroprot2 --mode collect-atoms --radius-classes ./resources/vdwr_classes.txt --radius-members ./resources/vdwr_members.txt --include-heteroatoms 0 --include-water 0 \
| \
./Release/voroprot2 --mode construct-inter-atom-contacts --depth 3 --probe 1.4
