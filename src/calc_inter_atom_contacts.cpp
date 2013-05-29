#include <iostream>

#include "protein/atom.h"

#include "apollo2/apollonius_triangulation.h"
#include "apollo/contact_surface.h"

#include "contacto/inter_atom_contact.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"

void calc_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--depth: --probe:");

	const std::size_t subdivision_depth=clo.isopt("--depth") ? clo.arg_in_interval<std::size_t>("--depth", 1, 4) : 3;
	const double probe_radius=clo.isopt("--probe") ? clo.arg_with_min_value<double>("--probe", 0) : 1.4;

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	if(atoms.size()<4)
	{
		throw std::runtime_error("Less than 4 atoms provided");
	}

	const std::vector< std::vector<std::size_t> > graph=apollo2::ApolloniusTriangulation::collect_neighbors_graph_from_neighbors_map(apollo2::ApolloniusTriangulation::collect_neighbors_map_from_quadruples_map(apollo2::ApolloniusTriangulation::construct_result(atoms, 3.5, false).quadruples_map), atoms.size());

	for(std::size_t i=0;i<graph.size();i++)
	{
		if(graph[i].empty())
		{
			std::clog << "Sphere was not included into the Voronoi diagram: " << atoms[i].string_for_human_reading() << "\n";
		}
	}

	const std::vector<contacto::InterAtomContact> inter_atom_contacts=apollo::ContactSurface::construct_inter_sphere_contacts_from_surface_areas<contacto::InterAtomContact>(
			apollo::ContactSurface::calculate_surface_areas(atoms, graph, subdivision_depth, probe_radius));

	if(atoms.empty() || inter_atom_contacts.empty())
	{
		throw std::runtime_error("No inter-atom contacts constructed");
	}
	else
	{
		auxiliaries::print_vector(std::cout, "atoms", atoms);
		auxiliaries::print_vector(std::cout, "contacts", inter_atom_contacts);
	}
}
