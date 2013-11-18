#!/bin/bash

( cat output1.txt | head -1 ; cat output1.txt | grep -v '#' ) > output2.txt
cat output2.txt | sed 's/:/: /g' | column -t > output3.txt
cat output3.txt | awk '{print $1 " " $2 " " $3 " " $4 " " $8 " " $26 " " $27}' | sed 's/://g' | sed 's/#//g' | column -t > output4.txt
cat output4.txt | sort -r | column -t > output5.txt
cat output5.txt | egrep -v '^m02_|^m03|^m04' | sed 's/m01_//g'  | sort -r | column -t > output6.txt