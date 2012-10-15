#include <iostream>
#include <map>
#include <limits>

#include "protein/atom.h"
#include "protein/residue_id.h"

#include "apollo/spheres_hierarchy.h"
#include "apollo/apollonius_triangulation.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/file_header.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/xml_writer.h"

void print_quadruples_image(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;

	clo.check_allowed_options("--min-r: --max-r: --k:");

	const double min_tsr=clo.isopt("--min-r") ? clo.arg_with_min_value<double>("--min-r", 0.0) : 0.0;
	const double max_tsr=clo.isopt("--max-r") ? clo.arg_with_min_value<double>("--max-r", 0.0) : std::numeric_limits<double>::max();
	const double k_tsr=clo.isopt("--k") ? clo.arg_with_min_value<double>("--k", 0.0) : 10;

	std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	if(atoms.size()<4)
	{
		throw std::runtime_error("Less than 4 atoms provided");
	}

	const Hierarchy hierarchy(atoms, 4.2, 1);
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, true);
	const Apollo::PairsNeighboursMap pairs_neighbours_map=Apollo::collect_pairs_neighbours_from_quadruples(quadruples_map);

	std::map<apollo::Pair, std::size_t> ordered_pairs_map;
	for(Apollo::PairsNeighboursMap::const_iterator it=pairs_neighbours_map.begin();it!=pairs_neighbours_map.end();++it)
	{
		ordered_pairs_map[it->first]=0;
	}
	{
		std::size_t i=0;
		for(std::map<apollo::Pair, std::size_t>::iterator it=ordered_pairs_map.begin();it!=ordered_pairs_map.end();++it)
		{
			it->second=i++;
		}
	}

	auxiliaries::XMLWriter svg("svg");
	svg.set("width", ordered_pairs_map.size()).set("height", ordered_pairs_map.size());
	for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
	{
		if(!it->second.empty())
		{
			const double tsr=it->second.front().r;
			if(tsr>=min_tsr && tsr<=max_tsr)
			{
				const apollo::Quadruple& q=it->first;
				const std::pair<std::size_t, std::size_t> p(ordered_pairs_map[apollo::make_pair(q.get(0), q.get(1))], ordered_pairs_map[apollo::make_pair(q.get(2), q.get(3))]);
				svg.add_child(auxiliaries::XMLWriter("circle").set("cx", p.first).set("cy", p.second).set("r", log(1+tsr)*k_tsr).set("fill", "blue"));
			}
		}
	}
	svg.write(std::cout);
}
