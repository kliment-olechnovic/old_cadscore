#ifndef PROTEIN_INTER_RESIDUE_CONTACTS_H_
#define PROTEIN_INTER_RESIDUE_CONTACTS_H_

#include <vector>
#include <map>

#include "protein/atom.h"
#include "protein/residue_id.h"
#include "contacto/inter_atom_contact.h"
#include "contacto/inter_residue_contact_id.h"
#include "contacto/inter_residue_contact_areas.h"
#include "contacto/inter_residue_contact_dual_areas.h"

std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactAreas > construct_inter_residue_contacts_from_inter_atom_contacts(
		const std::vector<protein::Atom>& atoms,
		const std::vector<contacto::InterAtomContact>& inter_atom_contacts);

std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactDualAreas > construct_combined_inter_residue_contacts_from_inter_atom_contacts(
		const std::vector<protein::Atom>& atoms_1,
		const std::vector<contacto::InterAtomContact>& inter_atom_contacts_1,
		const std::vector<protein::Atom>& atoms_2,
		const std::vector<contacto::InterAtomContact>& inter_atom_contacts_2);

#endif /* PROTEIN_INTER_RESIDUE_CONTACTS_H_ */
