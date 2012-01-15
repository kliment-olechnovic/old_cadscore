#include <iostream>

#include "protein/atom.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"

void main_construct_apollonius_quadrupalization(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;

	const double radius=clo.arg<double>("--radius", 1);
	const std::size_t low_count=clo.arg<double>("--low-count", 1);
	const int as_points=clo.arg<int>("--as-points", 0, 1);
	const int search_for_d3=clo.arg<int>("--search-for-d3", 0, 1);

	auxiliaries::assert_file_header("atoms");
	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>();

	std::clog << "atoms " << atoms.size() << "\n";

	if(as_points>0)
	{
		for(std::size_t i=0;i<atoms.size();i++)
		{
			atoms[i].r=0;
		}
	}

	const Hierarchy hierarchy(atoms, radius, low_count);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, search_for_d3>0);

	std::clog << "quadruples " << quadruples_map.size() << "\n";
	int vertices_count=0;
	for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
	{
		vertices_count+=it->second.size();
	}
	std::clog << "vertices " << vertices_count << "\n";

	auxiliaries::print_file_header("apollonius_quadruples");
	std::cout << quadruples_map.size() << "\n";
	for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
	{
		const apollo::Quadruple& q=it->first;
		std::cout << q.get(0) << " " << q.get(1) << " " << q.get(2) << " " << q.get(3) << "\n";
		const std::vector<apollo::SimpleSphere>& tangents=it->second;
		std::cout << tangents.size() << " ";
		for(std::size_t i=0;i<tangents.size();i++)
		{
			const apollo::SimpleSphere& s=tangents[i];
			std::cout << s.x << " " << s.y << " " << s.z << " " << s.r << ((i+1<tangents.size()) ? " " : "\n");
		}
	}

	if(clo.isopt("--check"))
	{
		std::clog << "validity ";
		if(Apollo::check_quadruples(quadruples_map, atoms))
		{
			std::clog << "good\n";
		}
		else
		{
			std::clog << "bad\n";
		}
	}
}
