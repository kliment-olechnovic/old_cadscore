#include "protein_inter_residue_contacts.h"

#include "contacto/inter_residue_contacts_basic_operations.h"

std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactAreas > construct_inter_residue_contacts_from_inter_atom_contacts(
		const std::vector<protein::Atom>& atoms,
		const std::vector<contacto::InterAtomContact>& inter_atom_contacts)
{
	return contacto::collect_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms, inter_atom_contacts);
}
