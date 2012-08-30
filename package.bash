#!/bin/bash

VERSION=$(hg branches -a | egrep '^default' | awk '{print $2}' | tr ':' '_')
PACKAGE_NAME="cadscore_$VERSION"

rm $PACKAGE_NAME.tar.gz
rm -r $PACKAGE_NAME

mkdir -p $PACKAGE_NAME
cp README $PACKAGE_NAME/README
cp LICENSE $PACKAGE_NAME/LICENSE
cp -r src $PACKAGE_NAME/src

mkdir -p $PACKAGE_NAME/bin
cp Release/voroprot2 $PACKAGE_NAME/bin/voroprot2
cp scripts/Voroprot2_calc.bash $PACKAGE_NAME/bin
cp scripts/Voroprot2_print_interfaces_as_PyMol_graphics.bash $PACKAGE_NAME/bin
cp scripts/CADscore_* $PACKAGE_NAME/bin

mkdir -p $PACKAGE_NAME/resources
cp resources/vdwr_* $PACKAGE_NAME/resources

tar -czf "$PACKAGE_NAME.tar.gz" $PACKAGE_NAME
rm -r $PACKAGE_NAME
