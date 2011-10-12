#include <iostream>
#include <fstream>

#include "protein/atoms_reading.h"
#include "apollo/apollonius_triangulation.h"
#include "monitor/BlockTimer.h"

int main()
{
	typedef protein::Atom Sphere;
	std::vector<Sphere> spheres;

	{
		monitor::BlockTimer bt("Atoms reading time");
		std::ifstream radius_classes_stream("resources/vdwr_classes.txt");
		std::ifstream radius_members_stream("resources/vdwr_members.txt");
		const protein::VanDerWaalsRadiusAssigner radius_assigner(radius_classes_stream, radius_members_stream);
		spheres=protein::AtomsReading::read_atoms_from_PDB_file_stream(std::cin, radius_assigner, false, false);
		std::clog << spheres.size() << " spheres read\n";
	}

	{
		monitor::BlockTimer bt("Apollo time");
		typedef apollo::SpheresHierarchy<Sphere> Hierarchy;
		const Hierarchy hierarchy(spheres, 1.4*3, 50);
		typedef apollo::ApolloniusTriangulation< apollo::SpheresHierarchy<Sphere> > Apollo;
		Apollo::QuadruplesMap quadruples_map=Apollo::find_quadruples(hierarchy);
		std::clog << quadruples_map.size() << " quadruples found\n";
//		std::clog << Apollo::check_quadruples(quadruples_map, spheres) << " quadruples status\n";

		{
			monitor::BlockTimer bth("Hidden spheres check time");
			std::clog << hierarchy.find_all_hidden_spheres().size() << " hidden spheres found\n";
		}

		{
			monitor::BlockTimer bth("Graph construction time");
			Apollo::Graph graph=Apollo::construct_graph_from_quadruples(quadruples_map);
		}

		{
			monitor::BlockTimer bth("Pairs neighbours construction time");
			Apollo::PairsNeighboursMap pairs_neighbours=Apollo::collect_pairs_neighbours_from_quadruples(quadruples_map);
		}

		{
			monitor::BlockTimer bth("Triples neighbours construction time");
			Apollo::TriplesNeighboursMap triples_neighbours=Apollo::collect_triples_neighbours_from_quadruples(quadruples_map);
		}
	}

	return 0;
}
