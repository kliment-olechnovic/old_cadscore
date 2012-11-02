#ifndef RESOURCES_VERSION_H_
#define RESOURCES_VERSION_H_

#include <string>

namespace resources
{

inline std::string get_version_string()
{
	static std::string version_string("CAD-score version 1029 db583b8a4091 (experimental)");
	return version_string;
}

}

#endif /* RESOURCES_VERSION_H_ */
