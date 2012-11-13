#!/bin/bash

cd $(dirname "$0")

VERSION=$(hg branches -a | egrep '^experimental' | awk '{print $2}' | head -1 | tr ':' ' ')
if [ -z "$VERSION" ]
then
  VERSION=$(hg branches -a | egrep '^default' | awk '{print $2}' | tr ':' ' ')
fi

VERSION_STRING="Version $VERSION (experimental)"

cat << EOF > ../src/resources/version.h
#ifndef RESOURCES_VERSION_H_
#define RESOURCES_VERSION_H_

#include <string>

namespace resources
{

inline std::string get_version_string()
{
	static std::string version_string("$VERSION_STRING");
	return version_string;
}

}

#endif /* RESOURCES_VERSION_H_ */
EOF
