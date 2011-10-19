#ifndef PROTEIN_INTER_ATOM_CONTACTS_H_
#define PROTEIN_INTER_ATOM_CONTACTS_H_

#include <vector>

#include "protein/atom.h"
#include "contacto/inter_atom_contact.h"

std::vector<contacto::InterAtomContact> construct_inter_atom_contacts_from_atoms_graph(
		const std::vector<protein::Atom>& atoms,
		const std::vector< std::vector<std::size_t> >& graph,
		const std::size_t subdivision_depth,
		const double probe_radius);

#endif /* PROTEIN_INTER_ATOM_CONTACTS_H_ */
