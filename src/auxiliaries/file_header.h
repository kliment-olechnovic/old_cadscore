#ifndef AUXILIARIES_FILE_HEADER_H_
#define AUXILIARIES_FILE_HEADER_H_

#include <string>
#include <iostream>
#include <stdexcept>

namespace auxiliaries
{

inline void print_file_header(std::ostream& out, const std::string& header)
{
	out << header << "\n";
}

inline bool check_file_header(std::istream& in, const std::string& header)
{
	std::string value;
	in >> value;
	return (header==value);
}

inline void assert_file_header(std::istream& in, const std::string& header)
{
	if(!check_file_header(in, header))
	{
		throw std::runtime_error(std::string("Missing file header: ")+header);
	}
}

}

#endif /* AUXILIARIES_FILE_HEADER_H_ */
