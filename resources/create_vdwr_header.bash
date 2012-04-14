#!/bin/bash

(echo -e '#ifndef VDWR_H_\n#define VDWR_H_\n' ; xxd -i vdwr_classes ; echo ; xxd -i vdwr_members ; echo -e '\n#endif /* VDWR_H_ */') > ../src/resources/vdwr.h
