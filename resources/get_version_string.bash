#!/bin/bash

cd $(dirname "$0")

VERSION=$(git rev-list --all --count)

echo "cadscore_1.${VERSION}"
