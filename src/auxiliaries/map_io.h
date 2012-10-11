#ifndef AUXILIARIES_MAP_IO_H_
#define AUXILIARIES_MAP_IO_H_

#include <map>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "file_header.h"

namespace auxiliaries
{

template<typename A, typename B>
void print_map(std::ostream& out, const std::string& header, const std::map<A, B>& map, const bool separate_with_new_line)
{
	if(!header.empty())
	{
		print_file_header(out, header);
	}

	out << map.size() << "\n";
	for(typename std::map<A, B>::const_iterator it=map.begin();it!=map.end();++it)
	{
		out << it->first << (separate_with_new_line ? "\n" : " ") << it->second << "\n";
	}
}

template<typename A, typename B>
std::map<A, B> read_map(std::istream& in, const std::string& name, const std::string& header, const bool allow_empty_result)
{
	std::ostringstream error_output;
	if(name.empty())
	{
		error_output << "Error reading map: ";
	}
	else
	{
		error_output << "Error reading map '" << name << "': ";
	}

	if(!in.good())
	{
		error_output << "non-readable input stream";
		throw std::runtime_error(error_output.str());
	}

	if(!header.empty())
	{
		if(!check_file_header(in, header))
		{
			error_output << "missing file header '" << header << "'";
			throw std::runtime_error(error_output.str());
		}
	}

	std::size_t n=0;
	in >> n;

	if(in.fail())
	{
		error_output << "bad format of input stream (missing map size)";
		throw std::runtime_error(error_output.str());
	}

	std::map<A, B> map;
	if(n>0)
	{
		typename std::map<A, B>::iterator prev=map.begin();
		for(std::size_t i=0;i<n;i++)
		{
			A key;
			in >> key;

			if(in.fail())
			{
				error_output << "bad format of input stream (failed to read key of element " << i << ")";
				throw std::runtime_error(error_output.str());
			}

			B value;
			in >> value;

			if(in.fail())
			{
				error_output << "bad format of input stream (failed to read value of element " << i << ")";
				throw std::runtime_error(error_output.str());
			}

			prev=map.insert(prev, std::make_pair(key, value));
		}
	}
	else
	{
		if(!allow_empty_result)
		{
			error_output << "no data in input stream";
			throw std::runtime_error(error_output.str());
		}
	}
	return map;
}

}

#endif /* AUXILIARIES_MAP_IO_H_ */
