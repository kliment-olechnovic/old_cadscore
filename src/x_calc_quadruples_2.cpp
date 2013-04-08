#include <iostream>
#include <map>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation_2.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"

void x_calc_quadruples_2(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation2<Hierarchy> Apollo;

	clo.check_allowed_options("--epsilon: --monitoring-level: --bsi-radius: --bsi-min-count: --as-points --skip-inner --check");

	if(clo.isopt("--epsilon"))
	{
		const double epsilon=clo.arg_with_min_value<double>("--epsilon", 0.0);
		apollo::epsilon_reference()=epsilon;
	}

	if(clo.isopt("--monitoring-level"))
	{
		const int monitoring_level=clo.arg_with_min_value<int>("--monitoring-level", 0);
		Apollo::monitoring_level_reference()=monitoring_level;
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

	const Hierarchy hierarchy(atoms, radius, low_count);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, search_for_e && !as_points);

	std::cout << "Atoms count: " << atoms.size() << "\n";
	std::cout << "Quadruples count: " << quadruples_map.size() << "\n";

	{
		int tangent_spheres_count=0;
		for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			tangent_spheres_count+=it->second.size();
		}
		std::cout << "Tangent spheres count: " << tangent_spheres_count << "\n";
	}

	typedef std::map< Apollo::QuadruplesMap::key_type, Apollo::QuadruplesMap::mapped_type > QuadruplesSortedMap;
	QuadruplesSortedMap quadruples_sorted_map;
	for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
	{
		quadruples_sorted_map.insert(*it);
	}

	for(QuadruplesSortedMap::const_iterator it=quadruples_sorted_map.begin();it!=quadruples_sorted_map.end();++it)
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

	if(clo.isopt("--check"))
	{
		if(!Apollo::check_quadruples(quadruples_map, atoms))
		{
			std::cerr << "Check failed!\n";
		}
	}
}
