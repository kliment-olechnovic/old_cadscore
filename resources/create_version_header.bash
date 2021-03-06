#!/bin/bash

cd $(dirname "$0")

VERSION_STRING=$(./get_version_string.bash)

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
