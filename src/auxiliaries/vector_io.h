#ifndef AUXILIARIES_VECTOR_IO_H_
#define AUXILIARIES_VECTOR_IO_H_

#include <vector>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "file_header.h"

namespace auxiliaries
{

template<typename T>
void print_vector(std::ostream& out, const std::string& header, const std::vector<T>& v)
{
	if(!header.empty())
	{
		print_file_header(out, header);
	}

	out << v.size() << "\n";
	for(std::size_t i=0;i<v.size();i++)
	{
		out << v[i] << "\n";
	}
}

template<typename T>
std::vector<T> read_vector(std::istream& in, const std::string& name, const std::string& header, const bool allow_empty_result)
{
	std::ostringstream error_output;
	if(name.empty())
	{
		error_output << "Error reading vector: ";
	}
	else
	{
		error_output << "Error reading vector '" << name << "': ";
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
		error_output << "bad format of input stream (missing vector size)";
		throw std::runtime_error(error_output.str());
	}

	if(n>0)
	{
		std::vector<T> v(n);
		for(std::size_t i=0;i<n;i++)
		{
			in >> v[i];

			if(in.fail())
			{
				error_output << "bad format of input stream (failed to read element " << i << ")";
				throw std::runtime_error(error_output.str());
			}
		}
		return v;
	}
	else
	{
		if(!allow_empty_result)
		{
			error_output << "no data in input stream";
			throw std::runtime_error(error_output.str());
		}
		return std::vector<T>();
	}
}

}

#endif /* AUXILIARIES_VECTOR_IO_H_ */
