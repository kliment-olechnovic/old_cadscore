#!/bin/bash

cd $(dirname "$0")

(echo -e '#ifndef VDWR_H_\n#define VDWR_H_\n\nnamespace resources\n{\n' ; xxd -i vdwr_classes ; echo ; xxd -i vdwr_members ; echo -e '\n}\n\n#endif /* VDWR_H_ */') > ../src/resources/vdwr.h
