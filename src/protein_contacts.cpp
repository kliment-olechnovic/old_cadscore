#include "protein_contacts.h"

#include "apollo/contact_surface.h"
#include "contacto/inter_atom_contact.h"
#include "contacto/inter_residue_contacts.h"

std::map< std::pair<protein::ResidueID, protein::ResidueID>, std::map<std::string, double> > collect_contacts_between_residues(
		const std::vector<protein::Atom>& atoms,
		const std::vector< std::vector<std::size_t> >& graph,
		const std::size_t subdivision_depth,
		const double probe_radius)
{
	return contacto::collect_contacts_between_residues<protein::Atom, protein::ResidueID>(
			atoms,
			apollo::ContactSurface::binary_contact_areas_from_surface_areas<contacto::InterAtomContact>(
					apollo::ContactSurface::calculate_surface_areas(atoms, graph, subdivision_depth, probe_radius))
	);
}
