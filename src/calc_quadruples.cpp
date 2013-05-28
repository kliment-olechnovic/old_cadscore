#include <iostream>

#include "protein/atom.h"

#include "apollo2/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"

void calc_quadruples(const auxiliaries::CommandLineOptions& clo)
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

	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

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

	const apollo2::ApolloniusTriangulation::Result apollonius_triangulation_result=apollo2::ApolloniusTriangulation::construct(atoms, bsi_init_radius, augment);

	if(!skip_output)
	{
		apollo2::ApolloniusTriangulation::print_quadruples_map(apollonius_triangulation_result.quadruples_map, std::cout);
	}

	if(print_log)
	{
		std::clog << "atoms " <<  atoms.size() << "\n";
		apollo2::ApolloniusTriangulation::print_result_log(apollonius_triangulation_result, std::clog);
	}

	if(check)
	{
		std::clog << "check " << (apollo2::ApolloniusTriangulation::check_quadruples_map(atoms, apollonius_triangulation_result.quadruples_map)) << "\n";
	}
}
