#!/bin/bash

./Release/voroprot2 --mode calculate-contact-area-difference-profile --intersect-sequences $1 --scoring-mode $2 \
| \
./Release/voroprot2 --mode calculate-contact-area-difference-global-score --use-min $3 --prefix $4
