#!/bin/bash

cd $(dirname "$0")

VERSION=$(hg branches | egrep '^experimental' | awk '{print $2}' | tr ':' ' ')

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
