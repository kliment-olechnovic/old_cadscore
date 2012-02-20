#ifndef COLUMNS_READER_H_
#define COLUMNS_READER_H_

#include <string>
#include <sstream>
#include <vector>
#include <map>

namespace auxiliaries
{

class ColumnsReader
{
public:
	ColumnsReader()
	{
	}

	void init(std::istream& input)
	{
		columns_.clear();
		std::vector<std::string> header=read_row(input);
		while(input.good())
		{
			const std::vector<std::string> row=read_row(input);
			if(row.size()==header.size())
			{
				for(std::size_t i=0;i<row.size();i++)
				{
					columns_[header[i]].push_back(row[i]);
				}
			}
		}
	}

	bool empty() const
	{
		return (columns_.empty() || columns_.begin()->second.empty());
	}

	std::size_t height() const
	{
		return (columns_.empty() ? 0 : columns_.begin()->second.size());
	}

	template<typename T>
	bool get_value(const std::string& column, const std::size_t index, T& value) const
	{
		std::map< std::string, std::vector<std::string> >::const_iterator it=columns_.find(column);
		if(it!=columns_.end())
		{
			const std::vector<std::string>& string_values=it->second;
			if(index<string_values.size())
			{
				value=convert_string<T>(string_values[index]);
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool get_values(const std::string& column, std::vector<T>& values) const
	{
		std::map< std::string, std::vector<std::string> >::const_iterator it=columns_.find(column);
		if(it!=columns_.end())
		{
			values=convert_strings<T>(it->second);
			return true;
		}
		return false;
	}

private:
	static std::vector<std::string> read_row(std::istream& input)
	{
		std::string line;
		getline(input, line);
		std::istringstream str_input(line);
		std::vector<std::string> result;
		while(str_input.good())
		{
			std::string token;
			str_input >> token;
			if(!token.empty())
			{
				result.push_back(token);
			}
		}
		return result;
	}

	template<typename T>
	static T convert_string(const std::string& str)
	{
		std::istringstream str_input(str);
		T val;
		str_input >> val;
		return val;
	}

	template<typename T>
	static std::vector<T> convert_strings(const std::vector<std::string>& strs)
	{
		std::vector<T> vals;
		for(std::size_t i=0;i<strs.size();i++)
		{
			vals.push_back(convert_string<T>(strs[i]));
		}
		return vals;
	}

	std::map< std::string, std::vector<std::string> > columns_;
};

}

#endif /* COLUMNS_READER_H_ */
