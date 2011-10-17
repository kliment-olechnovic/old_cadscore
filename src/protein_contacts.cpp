#include "protein_contacts.h"

#include "apollo/contact_surface.h"
#include "contacto/inter_atom_contact.h"
#include "contacto/inter_residue_contacts.h"

std::vector<contacto::InterAtomContact> inter_atom_contacts_from_surface_areas(const std::vector<apollo::ContactSurface::SurfaceArea>& surface_areas)
{
	std::vector<contacto::InterAtomContact> inter_atom_contacts;
	std::size_t contacts_count=0;
	for(std::size_t i=0;i<surface_areas.size();i++)
	{
		contacts_count+=surface_areas[i].size();
	}
	inter_atom_contacts.reserve(contacts_count);
	for(std::size_t i=0;i<surface_areas.size();i++)
	{
		const apollo::ContactSurface::SurfaceArea& surface_area=surface_areas[i];
		for(apollo::ContactSurface::SurfaceArea::const_iterator it=surface_area.begin();it!=surface_area.end();it++)
		{
			inter_atom_contacts.push_back(contacto::InterAtomContact(i, it->first, it->second));
		}
	}
	return inter_atom_contacts;
}

std::map< std::pair<protein::ResidueID, protein::ResidueID>, std::map<std::string, double> > collect_contacts_between_residues(
		const std::vector<protein::Atom>& atoms,
		const std::vector< std::vector<std::size_t> >& graph,
		const std::size_t subdivision_depth,
		const double probe_radius)
{
	return contacto::collect_contacts_between_residues<protein::Atom, protein::ResidueID>(
			atoms,
			inter_atom_contacts_from_surface_areas(
					apollo::ContactSurface::calculate_surface_areas(atoms, graph, subdivision_depth, probe_radius))
	);
}
