#ifndef COMMAND_LINE_OPTIONS_H_
#define COMMAND_LINE_OPTIONS_H_

#include <string>
#include <map>
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
			if(str.find("-")==0)
			{
				options_[str]="";
			}
			else if(i>0)
			{
				const std::string prev_str(argv[i-1]);
				if(prev_str.find("-")==0)
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
			throw std::invalid_argument(std::string("Missing command line argument: ")+name);
		}
		std::istringstream input(options_.find(name)->second);
		T value;
		input >> value;
		if(input.fail())
		{
			throw std::invalid_argument(std::string("Invalid command line argument: ")+name);
		}
		return value;
	}

private:
	std::map<std::string, std::string> options_;
};

}

#endif /* COMMAND_LINE_OPTIONS_H_ */
