#include "protein_inter_residue_contacts.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"
#include "auxiliaries/map_io.h"

void main_construct_inter_residue_contacts(const auxiliaries::CommandLineOptions& clo)
{
	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>();
	const std::vector<contacto::InterAtomContact> inter_atom_contacts=auxiliaries::read_vector<contacto::InterAtomContact>();
	const std::map< contacto::InterResidueContactID<protein::ResidueID>, contacto::InterResidueContactAreas > inter_residue_contacts=construct_inter_residue_contacts_from_inter_atom_contacts(atoms, inter_atom_contacts);
	auxiliaries::print_map(inter_residue_contacts);
}
