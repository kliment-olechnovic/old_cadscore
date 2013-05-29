#include <iostream>
#include <sstream>

#include "apollo2/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"

std::vector<apollo2::SimpleSphere> read_spheres_from_stream(std::istream& input)
{
	std::vector<apollo2::SimpleSphere> result;
	while(input.good())
	{
		std::string line;
		std::getline(input, line);
		if(!line.empty())
		{
			std::istringstream line_input(line);
			apollo2::SimpleSphere sphere;
			line_input >> sphere.x >> sphere.y >> sphere.z >> sphere.r;
			if(!line_input.fail())
			{
				result.push_back(sphere);
			}
		}
	}
	return result;
}

void x_calc_quadruples_2(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--epsilon: --bsi-init-radius: --use-one-radius --augment --skip-output --print-log --check");

	if(clo.isopt("--epsilon"))
	{
		const double epsilon=clo.arg_with_min_value<double>("--epsilon", 0.0);
		apollo2::comparison_epsilon_reference()=epsilon;
	}

	const double bsi_init_radius=clo.isopt("--bsi-init-radius") ? clo.arg_with_min_value<double>("--bsi-radius", 1) : 3.5;
	const bool use_one_radius=clo.isopt("--use-one-radius");
	const bool augment=clo.isopt("--augment");
	const bool skip_output=clo.isopt("--skip-output");
	const bool print_log=clo.isopt("--print-log");
	const bool check=clo.isopt("--check");

	std::vector<apollo2::SimpleSphere> atoms=read_spheres_from_stream(std::cin);

	if(atoms.size()<4)
	{
		throw std::runtime_error("Less than 4 atoms provided");
	}

	if(use_one_radius>0)
	{
		for(std::size_t i=0;i<atoms.size();i++)
		{
			atoms[i].r=1;
		}
	}

	const apollo2::ApolloniusTriangulation::Result apollonius_triangulation_result=apollo2::ApolloniusTriangulation::construct_result(atoms, bsi_init_radius, augment);

	if(!skip_output)
	{
		apollo2::ApolloniusTriangulation::print_quadruples_map(apollonius_triangulation_result.quadruples_map, std::cout);
	}

	if(print_log)
	{
		std::clog << "atoms " <<  atoms.size() << "\n";
		apollonius_triangulation_result.print_status(std::clog);
	}

	if(check)
	{
		std::clog << "check " << (apollo2::ApolloniusTriangulation::check_quadruples_map(atoms, apollonius_triangulation_result.quadruples_map)) << "\n";
	}
}
