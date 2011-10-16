#include <iostream>
#include <fstream>

#include "protein_input.h"
#include "protein_graph.h"

#include "apollo/contact_surface.h"
#include "monitor/BlockTimer.h"

int main()
{
	const std::vector<protein::Atom> unrefined_atoms=read_protein_atoms(false, false);
	const std::pair< std::vector<protein::Atom>, std::vector< std::vector<std::size_t> > > protein_graph=construct_protein_graph(unrefined_atoms);
	const std::vector<protein::Atom>& atoms=protein_graph.first;
	const std::vector< std::vector<std::size_t> >& graph=protein_graph.second;

	{
		monitor::BlockTimer bth("Contact surfaces construction time");
		std::vector<apollo::ContactSurface::SurfaceArea> surface_areas=apollo::ContactSurface::calculate_surface_areas(atoms, graph, 3, 1.4);
	}

	return 0;
}
