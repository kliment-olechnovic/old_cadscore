#ifndef AUXILIARIES_TABLE_H_
#define AUXILIARIES_TABLE_H_

#include <sstream>
#include <vector>
#include <map>

namespace auxiliaries
{

class Table
{
public:
	Table() : height_(0)
	{
	}

	Table(std::istream& input) : height_(0)
	{
		if(input.good())
		{
			std::string header_line;
			getline(input, header_line);
			const std::vector<std::string> column_names=parse_line(line);
			if(!column_names.empty())
			{
				while(input.good())
				{
					std::string line;
					getline(input, line);
					if(line[0]!='#')
					{
						const std::vector<std::string> values=parse_line(line);
						if(values.size()==column_names.size())
						{
							for(std::size_t i=0;i<column_names.size();i++)
							{
								columns_[column_names[i]].push_back(values[i]);
							}
							height_++;
						}
					}
				}
			}
		}
	}

	std::map< std::string, std::vector<string> >& columns() const
	{
		return columns_;
	}

	std::size_t height() const
	{
		return height_;
	}

	template<typename T>
	bool get_column(const std::string& column_name, T& values) const
	{
		std::map< std::string, std::vector<string> >::const_iterator it=columns_.find(column_name);
		if(it!=columns_.end())
		{
			std::vector<string>& string_values=it->second;
			values.resize(height_);
			for(std::size_t i=0;i<height_;i++)
			{
				std::istringstream input(string_values[i]);
				input >> values[i];
			}
			return true;
		}
		return false;
	}

	template<typename T>
	bool set_column(const std::string& column_name, const T& values)
	{
		if(!values.empty() && (height_==0 || values.size()==height_))
		{
			height_=values.size();
			std::vector<string>& string_values=columns_[column_name];
			string_values.resize(height_);
			for(std::size_t i=0;i<height_;i++)
			{
				std::ostringstream output;
				output << values[i];
				string_values[i]=output.str();
			}
			return true;
		}
		else
		{
			return false;
		}
	}

	void write(std::ostream& output) const
	{
		for(std::map< std::string, std::vector<string> >::const_iterator it=columns_.begin();it!=columns_.end();++it)
		{
			output << it->first << " ";
		}
		output << "\n";
		for(std::size_t i=0;i<height_;i++)
		{
			for(std::map< std::string, std::vector<string> >::const_iterator it=columns_.begin();it!=columns_.end();++it)
			{
				output << it->second[i] << " ";
			}
			output << "\n";
		}
	}

private:
	std::map< std::string, std::vector<string> > columns_;
	std::size_t height_;

	static std::vector<std::string> parse_line(const std::string& line)
	{
		std::vector<std::string> result;
		std::istringstream input(line);
		while(input.good())
		{
			std::string token;
			input >> token;
			if(!token.empty())
			{
				result.push_back(token);
			}
		}
		return result;
	}
};

}


#endif /* AUXILIARIES_TABLE_H_ */
