#ifndef AUXILIARIES_SET_IO_H_
#define AUXILIARIES_SET_IO_H_

#include <set>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "file_header.h"

namespace auxiliaries
{

template<typename T>
void print_set(std::ostream& out, const std::string& header, const std::set<T>& set)
{
	if(!header.empty())
	{
		print_file_header(out, header);
	}

	out << set.size() << "\n";
	for(typename std::set<T>::const_iterator it=set.begin();it!=set.end();++it)
	{
		out << (*it) << "\n";
	}
}

template<typename T>
std::set<T> read_set(std::istream& in, const std::string& name, const std::string& header, const bool allow_empty_result)
{
	std::ostringstream error_output;
	if(name.empty())
	{
		error_output << "Error reading set: ";
	}
	else
	{
		error_output << "Error reading set '" << name << "': ";
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
		error_output << "bad format of input stream (missing set size)";
		throw std::runtime_error(error_output.str());
	}

	std::set<T> set;
	if(n>0)
	{
		typename std::set<T>::iterator prev=set.begin();
		for(std::size_t i=0;i<n;i++)
		{
			T value;
			in >> value;

			if(in.fail())
			{
				error_output << "bad format of input stream (failed to read element " << i << ")";
				throw std::runtime_error(error_output.str());
			}

			prev=set.insert(prev, value);
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
	return set;
}

}

#endif /* AUXILIARIES_SET_IO_H_ */
