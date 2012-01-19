#include <iostream>

#include "../protein/atom.h"

#include "../apollo/spheres_hierarchy.h"
#include "../apollo/apollonius_triangulation.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/vector_io.h"

void main_construct_apollonius_quadrupalization(const auxiliaries::CommandLineOptions& clo)
{
	typedef apollo::SpheresHierarchy<protein::Atom> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy, 1> Apollo;

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
	const Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy, search_for_d3>0 && !as_points);

	{
		int vertices_count=0;
		for(Apollo::QuadruplesMap::const_iterator it=quadruples_map.begin();it!=quadruples_map.end();++it)
		{
			vertices_count+=it->second.size();
		}
		std::clog << "vertices " << vertices_count << "\n";
	}

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

	if(clo.isopt("--check-quadruples"))
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

	if(clo.isopt("--check-for-orphans"))
	{
		std::clog << "orphans ";
		const std::vector< std::vector<std::size_t> > graph=Apollo::construct_graph_from_quadruples(quadruples_map, atoms.size());
		std::vector<std::size_t> orphans;
		for(std::size_t i=0;i<graph.size();i++)
		{
			if(graph[i].empty())
			{
				orphans.push_back(i);
			}
		}
		std::clog << orphans.size() << "\n";
	}

	if(clo.isopt("--check-euler-formula"))
	{
		const Apollo::TriplesNeighboursMap triples_map=Apollo::collect_triples_neighbours_from_quadruples(quadruples_map);
		std::size_t faces_count=0;
		std::tr1::unordered_set<apollo::Pair, apollo::Pair::HashFunctor> edges_set;
		std::tr1::unordered_set<std::size_t> vertices_set;
		for(Apollo::TriplesNeighboursMap::const_iterator it=triples_map.begin();it!=triples_map.end();it++)
		{
			if(it->second.size()==1)
			{
				faces_count++;
				for(int i=0;i<3;i++)
				{
					edges_set.insert(it->first.exclude(i));
					vertices_set.insert(it->first.get(i));
				}
			}
		}
		std::clog << "euler " << (vertices_set.size()+faces_count-edges_set.size()) << "\n";
	}
}
