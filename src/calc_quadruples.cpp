#include <iostream>

#include "protein/atom.h"

#include "apollo2/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/std_containers_io.h"

namespace
{

template<typename SphereType>
void calc_quadruples(
		const std::vector<SphereType>& input_atoms,
		const double bsi_init_radius,
		const bool use_one_radius,
		const bool augment,
		const bool skip_output,
		const bool print_log,
		const bool check)
{
	if(input_atoms.size()<4)
	{
		throw std::runtime_error("Less than 4 atoms provided");
	}

	const std::vector<SphereType>* atoms_ptr=(&input_atoms);

	std::vector<SphereType> modified_atoms;
	if(use_one_radius)
	{
		modified_atoms=input_atoms;
		for(std::size_t i=0;i<modified_atoms.size();i++)
		{
			modified_atoms[i].r=1;
		}
		atoms_ptr=(&modified_atoms);
	}

	const std::vector<SphereType>& atoms=(*atoms_ptr);

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

}

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

	const std::vector<protein::Atom> atoms=auxiliaries::STDContainersIO::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	calc_quadruples(atoms, bsi_init_radius, use_one_radius, augment, skip_output, print_log, check);
}
