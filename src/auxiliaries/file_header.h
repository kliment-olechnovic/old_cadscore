#ifndef FILE_HEADER_H_
#define FILE_HEADER_H_

#include <string>
#include <iostream>
#include <stdexcept>

namespace auxiliaries
{

inline void print_file_header(const std::string& header)
{
	std::cout << header << "\n";
}

inline void assert_file_header(const std::string& header)
{
	std::string value;
	std::cin >> value;
	if(header!=value)
	{
		throw std::runtime_error(std::string("Missing file header: ")+header);
	}
}

}

#endif /* FILE_HEADER_H_ */
