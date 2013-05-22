#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include "apollo2/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"

std::vector<apollo2::SimpleSphere> read_spheres_from_stream(std::istream& input);

std::vector<apollo2::apollonius_triangulation::Quadruple> read_quadruples_from_stream(std::istream& input)
{
	std::vector<apollo2::apollonius_triangulation::Quadruple> result;
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
				result.push_back(apollo2::apollonius_triangulation::Quadruple(numbers));
			}
		}
	}
	std::sort(result.begin(), result.end());
	return result;
}

void x_compare_two_sets_of_quadruples_2(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--file1: --file2:");

	std::string file1=clo.arg<std::string>("--file1");
	std::string file2=clo.arg<std::string>("--file2");

	std::vector<apollo2::SimpleSphere> atoms=read_spheres_from_stream(std::cin);
	apollo2::BoundingSpheresHierarchy<apollo2::SimpleSphere> bsh(atoms, 3.5, 1);

	std::ifstream stream1(file1.c_str(), std::ifstream::in);
	std::ifstream stream2(file2.c_str(), std::ifstream::in);

	std::vector<apollo2::apollonius_triangulation::Quadruple> quadruples1=read_quadruples_from_stream(stream1);
	std::vector<apollo2::apollonius_triangulation::Quadruple> quadruples2=read_quadruples_from_stream(stream2);

	std::vector<apollo2::apollonius_triangulation::Quadruple> difference(quadruples1.size());
	std::vector<apollo2::apollonius_triangulation::Quadruple>::iterator it=std::set_difference(quadruples1.begin(), quadruples1.end(), quadruples2.begin(), quadruples2.end(), difference.begin());
	difference.resize(it-difference.begin());

	std::vector<apollo2::apollonius_triangulation::Quadruple> unconfirmed_difference;
	std::vector<apollo2::apollonius_triangulation::Quadruple> confirmed_difference;
	for(std::size_t i=0;i<difference.size();i++)
	{
		apollo2::apollonius_triangulation::Quadruple q=difference[i];
		if(q.get(0)<atoms.size() && q.get(1)<atoms.size() && q.get(2)<atoms.size() && q.get(3)<atoms.size())
		{
			const std::vector<apollo2::SimpleSphere> tangents=apollo2::apollonius_triangulation::TangentSphereOfFourSpheres::calculate<apollo2::SimpleSphere>(atoms.at(q.get(0)), atoms.at(q.get(1)), atoms.at(q.get(2)), atoms.at(q.get(3)));
			if(tangents.size()>0)
			{
				if(apollo2::apollonius_triangulation::find_any_collision(bsh, tangents[0]).size()>0)
				{
					confirmed_difference.push_back(q);
				}
				else if(tangents.size()==2 && apollo2::apollonius_triangulation::find_any_collision(bsh, tangents[1]).size()>0)
				{
					confirmed_difference.push_back(q);
				}
				else
				{
					unconfirmed_difference.push_back(q);
				}
			}
			else
			{
				unconfirmed_difference.push_back(q);
			}
		}
	}

	std::cout << "unconfirmed_difference " << unconfirmed_difference.size() << "\n";
	std::cout << "confirmed_difference " << confirmed_difference.size() << "\n";
}
