#include <iostream>

#include "atoms/atoms.h"
#include "../../contactus/src/baltymus/utils/BlockTimer.h"

#include "apollo/apollonius_triangulation.h"

template<typename SphereType>
void print_sphere(const SphereType& sphere)
{
	std::cout << "SPHERE " << sphere.r << " " << sphere.x << " " << sphere.y << " " << sphere.z << "\n";
}

template<typename Iterator>
void print_spheres(const Iterator first, const Iterator last)
{
	for(Iterator it=first;it!=last;++it)
	{
		print_sphere(*it);
	}
}

int main()
{
	typedef Atom Sphere;
	std::vector<Sphere> spheres;

	{
		utils::BlockTimer bt("Reading time");
		spheres=read_atoms_from_PDB_file_stream(std::cin, false, false);
		std::clog << spheres.size() << " spheres read\n";
	}

	{
		utils::BlockTimer bt("Apollo time");
		typedef apollo::SpheresHierarchy<Sphere> Hierarchy;
		const Hierarchy hierarchy(spheres, 1.4*3, 50);
		typedef apollo::ApolloniusTriangulation< apollo::SpheresHierarchy<Sphere> > Apollo;
		Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy);
		std::clog << quadruples_map.size() << " quadruples found\n";
//		std::clog << Apollo::check_quadruples(quadruples_map, spheres) << " quadruples status\n";

		{
			utils::BlockTimer bth("Hidden spheres check time");
			std::clog << hierarchy.find_all_hidden_spheres().size() << " hidden spheres found\n";
		}

		{
			utils::BlockTimer bth("Graph construction time");
			Apollo::Graph graph=Apollo::construct_graph_from_quadruples(quadruples_map);
		}

		{
			utils::BlockTimer bth("Pairs neighbours construction time");
			Apollo::PairsNeighboursMap pairs_neighbours=Apollo::collect_pairs_neighbours_from_quadruples(quadruples_map);
		}

		{
			utils::BlockTimer bth("Triples neighbours construction time");
			Apollo::TriplesNeighboursMap triples_neighbours=Apollo::collect_triples_neighbours_from_quadruples(quadruples_map);
		}
	}

	return 0;
}
