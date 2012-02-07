#ifndef AUXILIARIES_COMMAND_LINE_OPTIONS_H_
#define AUXILIARIES_COMMAND_LINE_OPTIONS_H_

#include <string>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>

namespace auxiliaries
{

class CommandLineOptions
{
public:
	CommandLineOptions(const int argc, const char** argv)
	{
		for(int i=1;i<argc;i++)
		{
			const std::string str(argv[i]);
			if(str.find("--")==0)
			{
				options_[str]="";
			}
			else if(i>0)
			{
				const std::string prev_str(argv[i-1]);
				if(prev_str.find("--")==0)
				{
					options_[prev_str]=str;
				}
			}
		}
	}

	bool isopt(const std::string& name) const
	{
		return (options_.find(name)!=options_.end());
	}

	bool isarg(const std::string& name) const
	{
		return (isopt(name) && !options_.find(name)->second.empty());
	}

	template<typename T>
	T arg(const std::string& name) const
	{
		if(!isarg(name))
		{
			throw std::runtime_error(std::string("Missing command line argument: ")+name);
		}
		std::istringstream input(options_.find(name)->second);
		T value;
		input >> value;
		if(input.fail())
		{
			throw std::runtime_error(std::string("Invalid command line argument: ")+name);
		}
		return value;
	}

	template<typename T>
	T arg_in_interval(const std::string& name, const T min_value, const T max_value) const
	{
		const T value=arg<T>(name);
		if(!(value>=min_value && value<=max_value))
		{
			std::ostringstream output;
			output << "Command line argument '" << name << "' is not in the allowed range [" << min_value << ", " << max_value << "]";
			throw std::runtime_error(output.str());
		}
		return value;
	}

	template<typename T>
	T arg_with_min_value(const std::string& name, const T min_value) const
	{
		const T value=arg<T>(name);
		if(value<min_value)
		{
			std::ostringstream output;
			output << "Command line argument '" << name << "' is less than " << min_value;
			throw std::runtime_error(output.str());
		}
		return value;
	}

	template<typename T>
	T arg_or_default_value(const std::string& name, const T default_value) const
	{
		if(isopt(name))
		{
			return arg<T>(name);
		}
		else
		{
			return default_value;
		}
	}

	void check_allowed_options(const std::string& allowed_options)
	{
		std::istringstream input(allowed_options);
		std::set<std::string> allowed_options_set_0;
		std::set<std::string> allowed_options_set_1;
		while(input.good())
		{
			std::string token;
			input >> token;
			if(token.find(":")==std::string::npos)
			{
				allowed_options_set_0.insert(token);
			}
			else
			{
				allowed_options_set_1.insert(token.substr(0, token.find(":")));
			}
		}
		for(std::map<std::string, std::string>::const_iterator it=options_.begin();it!=options_.end();++it)
		{
			const std::string& option=it->first;
			//TODO finish this
			if(it->second.empty())
			{
				//
			}
			else
			{
				//
			}
//			if(allowed_options_set_0.count(option)==0)
//			{
//				throw std::runtime_error(std::string("Unrecognized command line option: ")+option);
//			}
		}
	}

private:
	std::map<std::string, std::string> options_;
};

}

#endif /* AUXILIARIES_COMMAND_LINE_OPTIONS_H_ */
