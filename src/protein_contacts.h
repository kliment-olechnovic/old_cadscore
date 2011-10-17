#ifndef PROTEIN_CONTACTS_H_
#define PROTEIN_CONTACTS_H_

#include <vector>
#include <map>

#include "contacto/inter_atom_contact.h"
#include "protein/atom.h"
#include "protein/residue_id.h"

std::vector<contacto::InterAtomContact> construct_inter_atom_contacts_from_atoms_graph(
		const std::vector<protein::Atom>& atoms,
		const std::vector< std::vector<std::size_t> >& graph,
		const std::size_t subdivision_depth,
		const double probe_radius);

std::map< std::pair<protein::ResidueID, protein::ResidueID>, std::map<std::string, double> > collect_contacts_between_residues(
		const std::vector<protein::Atom>& atoms,
		const std::vector<contacto::InterAtomContact>& inter_atom_contacts);

#endif /* PROTEIN_CONTACTS_H_ */
