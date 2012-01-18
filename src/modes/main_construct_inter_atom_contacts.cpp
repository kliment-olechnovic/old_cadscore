#include <iostream>

#include "../protein/atom.h"

#include "../apollo/spheres_hierarchy.h"
#include "../apollo/apollonius_triangulation.h"
#include "../apollo/contact_surface.h"

#include "../contacto/inter_atom_contact.h"

#include "../auxiliaries/command_line_options.h"
#include "../auxiliaries/file_header.h"
#include "../auxiliaries/vector_io.h"

template<typename SphereType>
std::pair< std::vector<SphereType>, std::vector< std::vector<std::size_t> > > construct_spheres_graph(const std::vector<SphereType>& unrefined_spheres, const double r, const std::size_t low_count)
{
	typedef apollo::SpheresHierarchy<SphereType> Hierarchy;
	typedef apollo::ApolloniusTriangulation<Hierarchy> Apollo;

	const Hierarchy hierarchy(unrefined_spheres, r, low_count);

	const std::set<std::size_t> hidden_spheres=hierarchy.find_all_hidden_spheres();

	for(std::set<std::size_t>::const_iterator it=hidden_spheres.begin();it!=hidden_spheres.end();++it)
	{
		std::clog << "Sphere is fully inside some other sphere: " << unrefined_spheres[*it] << "\n";
	}

	if(hidden_spheres.empty())
	{
		const std::vector< std::vector<std::size_t> > graph=Apollo::construct_graph_from_quadruples(Apollo::find_quadruples(hierarchy, true), unrefined_spheres.size());
		return std::make_pair(unrefined_spheres, graph);
	}
	else
	{
		std::vector<SphereType> refined_spheres;
		refined_spheres.reserve(unrefined_spheres.size()-hidden_spheres.size());
		for(std::size_t i=0;i<unrefined_spheres.size();i++)
		{
			if(hidden_spheres.find(i)==hidden_spheres.end())
			{
				refined_spheres.push_back(unrefined_spheres[i]);
			}
		}

		const Hierarchy refined_hierarchy(refined_spheres, r, low_count);
		const std::vector< std::vector<std::size_t> > graph=Apollo::construct_graph_from_quadruples(Apollo::find_quadruples(refined_hierarchy, true), refined_spheres.size());
		return std::make_pair(refined_spheres, graph);
	}
}

void main_construct_inter_atom_contacts(const auxiliaries::CommandLineOptions& clo)
{
	const std::size_t subdivision_depth=clo.arg<std::size_t>("--depth", 1, 4);
	const double probe_radius=clo.arg<double>("--probe", 0);

	auxiliaries::assert_file_header("atoms");
	const std::vector<protein::Atom> unrefined_atoms=auxiliaries::read_vector<protein::Atom>();

	const std::pair< std::vector<protein::Atom>, std::vector< std::vector<std::size_t> > > protein_graph=construct_spheres_graph(unrefined_atoms, 1.4*3, 50);
	const std::vector<protein::Atom>& atoms=protein_graph.first;
	const std::vector< std::vector<std::size_t> >& graph=protein_graph.second;

	for(std::size_t i=0;i<graph.size();i++)
	{
		if(graph[i].empty())
		{
			std::clog << "Atom has no neighbours in the Apollonius graph: " << atoms[i] << "\n";
		}
	}

	const std::vector<contacto::InterAtomContact> inter_atom_contacts=apollo::ContactSurface::construct_inter_sphere_contacts_from_surface_areas<contacto::InterAtomContact>(
			apollo::ContactSurface::calculate_surface_areas(atoms, graph, subdivision_depth, probe_radius));

	auxiliaries::print_file_header("atoms");
	auxiliaries::print_vector(atoms);

	auxiliaries::print_file_header("contacts");
	auxiliaries::print_vector(inter_atom_contacts);
}
