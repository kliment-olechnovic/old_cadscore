#!/bin/bash

cat vdwr_classes.txt | ( echo "static unsigned char vdwr_classes[] = {"; xxd -i; echo "};" ) > resources.cpp
echo >> resources.cpp
cat vdwr_members.txt | ( echo "static unsigned char vdwr_members[] = {"; xxd -i; echo "};" ) >> resources.cpp
