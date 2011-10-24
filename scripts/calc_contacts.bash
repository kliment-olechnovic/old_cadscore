#!/bin/bash

./voroprot --mode collect-atoms --radius-classes ./resources/vdwr_classes.txt --radius-members ./resources/vdwr_members.txt --include-heteroatoms 0 --include-water 0 \
| \
./voroprot --mode construct-inter-atom-contacts --depth 3 --probe 1.4
