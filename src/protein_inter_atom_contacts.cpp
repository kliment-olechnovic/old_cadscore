#include "protein_inter_atom_contacts.h"

#include "apollo/contact_surface.h"

std::vector<contacto::InterAtomContact> construct_inter_atom_contacts_from_atoms_graph(
		const std::vector<protein::Atom>& atoms,
		const std::vector< std::vector<std::size_t> >& graph,
		const std::size_t subdivision_depth,
		const double probe_radius)
{
	return apollo::ContactSurface::construct_inter_sphere_contacts_from_surface_areas<contacto::InterAtomContact>(
			apollo::ContactSurface::calculate_surface_areas(atoms, graph, subdivision_depth, probe_radius));
}
