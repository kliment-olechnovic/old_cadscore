#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

void calc_quadruples(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;

	clo.check_allowed_options("--mode: --epsilon: --bsi-radius: --bsi-min-count: --as-points --skip-inner");

	if(clo.isopt("--epsilon"))
	{
		const double epsilon=clo.arg_with_min_value<double>("--epsilon", 0.0);
		apollo::epsilon_reference()=epsilon;
	}

	const double radius=clo.isopt("--bsi-radius") ? clo.arg_with_min_value<double>("--bsi-radius", 1) : 4.2;
	const std::size_t low_count=clo.isopt("--bsi-min-count") ? clo.arg_with_min_value<std::size_t>("--bsi-min-count", 1) : 1;
	const bool as_points=clo.isopt("--as-points");
	const bool search_for_d3=!clo.isopt("--skip-inner");

	auxiliaries::assert_file_header(std::cin, "atoms");
	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin);

	if(as_points>0)
	{
		for(std::size_t i=0;i<atoms.size();i++)
		{
			atoms[i].r=0;
		}
	}

	const Hierarchy hierarchy(atoms, radius, low_count);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, search_for_d3 && !as_points);

	std::cout << "Quadruples count: " << quadruples_map.size() << "\n";

	{
		int tangent_spheres_count=0;
		for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			tangent_spheres_count+=it->second.size();
		}
		std::cout << "Tangent spheres count: " << tangent_spheres_count << "\n";
	}

	for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
	{
		std::cout << "\n";
		const apollo::Quadruple& q=it->first;
		std::cout << "Quadruple (a, b, c, d): " << q.get(0) << " " << q.get(1) << " " << q.get(2) << " " << q.get(3) << "\n";
		const std::vector<apollo::SimpleSphere>& tangents=it->second;
		for(std::size_t i=0;i<tangents.size();i++)
		{
			const apollo::SimpleSphere& s=tangents[i];
			std::cout << "Tangent sphere (x, y, z, r): " << s.x << " " << s.y << " " << s.z << " " << s.r << "\n";
		}
	}
}

