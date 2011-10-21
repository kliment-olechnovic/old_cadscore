#include "protein_inter_residue_contacts.h"

#include "contacto/inter_residue_contacts_construction.h"
#include "contacto/inter_residue_contacts_combination.h"

std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactAreas > construct_inter_residue_contacts_from_inter_atom_contacts(
		const std::vector<protein::Atom>& atoms,
		const std::vector<contacto::InterAtomContact>& inter_atom_contacts)
{
	return contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms, inter_atom_contacts);
}

std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > construct_combined_inter_residue_contacts_from_inter_atom_contacts(
		const std::vector<protein::Atom>& atoms_1,
		const std::vector<contacto::InterAtomContact>& inter_atom_contacts_1,
		const std::vector<protein::Atom>& atoms_2,
		const std::vector<contacto::InterAtomContact>& inter_atom_contacts_2)
{
	return contacto::InterResidueContactsCombination::combine_two_inter_residue_contact_maps<protein::ResidueID>(
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_1, inter_atom_contacts_1),
			contacto::construct_inter_residue_contacts<protein::Atom, protein::ResidueID>(atoms_2, inter_atom_contacts_2));
}
