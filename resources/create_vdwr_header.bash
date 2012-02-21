#!/bin/bash

(echo -e '#ifndef VDWR_H_\n#define VDWR_H_\n' ; xxd -i vdwr_classes.txt ; echo ; xxd -i vdwr_members.txt ; echo -e '\n#endif /* VDWR_H_ */') > ../src/resources/vdwr.h
