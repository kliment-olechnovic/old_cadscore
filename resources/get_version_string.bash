#!/bin/bash

cd $(dirname "$0")

VERSION=$(hg branches | egrep '^default' | awk '{print $2}' | tr ':' '_')

echo "cadscore_"$VERSION
