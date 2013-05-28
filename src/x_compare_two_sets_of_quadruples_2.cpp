#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

#include "apollo2/apollonius_triangulations_comparison.h"

#include "auxiliaries/command_line_options.h"

std::vector<apollo2::SimpleSphere> read_spheres_from_stream(std::istream& input);

std::vector<apollo2::Quadruple> read_quadruples_from_stream(std::istream& input)
{
	std::vector<apollo2::Quadruple> result;
	while(input.good())
	{
		std::string line;
		std::getline(input, line);
		if(!line.empty())
		{
			std::istringstream line_input(line);
			std::vector<std::size_t> numbers(4, 0);
			for(std::size_t i=0;i<numbers.size() && !line_input.fail();i++)
			{
				line_input >> numbers[i];
			}
			if(!line_input.fail())
			{
				result.push_back(apollo2::Quadruple(numbers));
			}
		}
	}
	if(!result.empty())
	{
		std::sort(result.begin(), result.end());
		std::vector<apollo2::Quadruple>::iterator it=std::unique(result.begin(), result.end());
		result.resize(it-result.begin());
	}
	return result;
}

void x_compare_two_sets_of_quadruples_2(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--file1: --file2:");

	std::string file1=clo.arg<std::string>("--file1");
	std::string file2=clo.arg<std::string>("--file2");

	std::vector<apollo2::SimpleSphere> atoms=read_spheres_from_stream(std::cin);

	std::ifstream stream1(file1.c_str(), std::ifstream::in);
	std::ifstream stream2(file2.c_str(), std::ifstream::in);

	std::vector<apollo2::Quadruple> quadruples1=read_quadruples_from_stream(stream1);
	std::vector<apollo2::Quadruple> quadruples2=read_quadruples_from_stream(stream2);

	const apollo2::ApolloniusTriangulationsComparison::Result differences=apollo2::ApolloniusTriangulationsComparison::calculate_directional_difference_between_two_sets_of_quadruples(atoms, 3.5, quadruples1, quadruples2);

	std::cout << "all_differences " << differences.all_differences.size() << "\n";
	std::cout << "confirmed_differences " << differences.confirmed_differences.size() << "\n";
}
