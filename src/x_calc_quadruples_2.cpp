#include <iostream>
#include <map>

#include "protein/atom.h"

#include "apollo2/bounding_spheres_hierarchy.h"
#include "apollo2/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"

void x_calc_quadruples_2(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--epsilon: --bsi-radius: --bsi-min-count: --as-points --skip-inner --monitor");

	if(clo.isopt("--epsilon"))
	{
		const double epsilon=clo.arg_with_min_value<double>("--epsilon", 0.0);
		apollo2::comparison_epsilon_reference()=epsilon;
	}

	const double radius=clo.isopt("--bsi-radius") ? clo.arg_with_min_value<double>("--bsi-radius", 1) : 4.2;
	const std::size_t low_count=clo.isopt("--bsi-min-count") ? clo.arg_with_min_value<std::size_t>("--bsi-min-count", 1) : 1;
	const bool as_points=clo.isopt("--as-points");
	const bool search_for_e=!clo.isopt("--skip-inner");

	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	if(atoms.size()<4)
	{
		throw std::runtime_error("Less than 4 atoms provided");
	}

	if(as_points>0)
	{
		for(std::size_t i=0;i<atoms.size();i++)
		{
			atoms[i].r=0;
		}
	}

	const apollo2::BoundingSpheresHierarchy<protein::Atom> hierarchy(atoms, radius, low_count);
	apollo2::ApolloniusTriangulation::print_map_of_quadruples(apollo2::ApolloniusTriangulation::construct_map_of_quadruples(hierarchy, search_for_e && !as_points), std::cout);

	if(clo.isopt("--monitor"))
	{
		apollo2::ApolloniusTriangulation::log().print(std::clog);
	}
}

