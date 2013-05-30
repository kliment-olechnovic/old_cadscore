#!/bin/bash

PACKAGE_NAME=$1

cd $(dirname "$0")

VERSION_STRING=$(./resources/get_version_string.bash)

if [ -z "$PACKAGE_NAME" ]
then
  PACKAGE_NAME=$VERSION_STRING
fi

rm -f $PACKAGE_NAME.tar.gz
rm -r -f $PACKAGE_NAME

mkdir -p $PACKAGE_NAME
echo $VERSION_STRING > $PACKAGE_NAME/VERSION
cp README $PACKAGE_NAME/README
cp LICENSE $PACKAGE_NAME/LICENSE
cp -r src $PACKAGE_NAME/src

mkdir -p $PACKAGE_NAME/bin
cp Release/voroprot2 $PACKAGE_NAME/bin/voroprot2
cp scripts/Voroprot2_* $PACKAGE_NAME/bin
cp scripts/CADscore_* $PACKAGE_NAME/bin
cp scripts/TMscore_calc.bash $PACKAGE_NAME/bin

mkdir -p $PACKAGE_NAME/resources
cp resources/vdwr_* $PACKAGE_NAME/resources

tar -czf "$PACKAGE_NAME.tar.gz" $PACKAGE_NAME
rm -r $PACKAGE_NAME
