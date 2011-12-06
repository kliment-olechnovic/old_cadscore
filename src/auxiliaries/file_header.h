#ifndef AUXILIARIES_FILE_HEADER_H_
#define AUXILIARIES_FILE_HEADER_H_

#include <string>
#include <iostream>
#include <stdexcept>

namespace auxiliaries
{

inline void print_file_header(const std::string& header)
{
	std::cout << header << "\n";
}

inline bool check_file_header(const std::string& header)
{
	std::string value;
	std::cin >> value;
	return (header==value);
}

inline void assert_file_header(const std::string& header)
{
	if(!check_file_header(header))
	{
		throw std::runtime_error(std::string("Missing file header: ")+header);
	}
}

}

#endif /* AUXILIARIES_FILE_HEADER_H_ */
